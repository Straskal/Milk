#include "run.h"

#include <iostream>
#include <limits>

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "SDL.h"

#include "Locator.h"
#include "luamlib.h"

#include "audio/sdl/SDLAudioPlayer.h"
#include "audio/sdl/SDLMusicCache.h"
#include "audio/sdl/SDLSoundCache.h"
#include "graphics/Color.h"
#include "graphics/sdl/SDLRenderer.h"
#include "graphics/sdl/SDLImageCache.h"
#include "keyboard/sdl/SDLKeyboard.h"
#include "mouse/sdl/SDLMouse.h"
#include "time/Time.h"
#include "window/sdl/SDLWindow.h"

#define free_ptr(x) delete x; x = nullptr
#define deinit_and_free_ptr(x) x->free(); free_ptr(x)

static const int MILK_SUCCESS = 0;
static const int MILK_FAIL = 1;
static const int ERROR_HANDLER_STACK_INDEX = 1;
static const int CALLBACK_TABLE_STACK_INDEX = 2;
static const double TICK_RATE = 1.0 / 60.0;

static milk::Time* time = nullptr;
static milk::SDLWindow* window = nullptr;
static milk::SDLRenderer* renderer = nullptr;
static milk::SDLMouse* mouse = nullptr;
static milk::SDLKeyboard* keyboard = nullptr;
static milk::SDLImageCache* image_cache = nullptr;
static milk::SDLAudioPlayer* audio_player = nullptr;
static milk::SDLSoundCache* sound_cache = nullptr;
static milk::SDLMusicCache* music_cache = nullptr;
static lua_State* lua = nullptr;

static int error_handler(lua_State* L)
{
	luaL_traceback(L, L, lua_tostring(L, 1), 0);
	return 1;
}

static void print_runtime_error(const char* err)
{
	// Minimize window before displaying the error and locking the event loop.
	// This basically avoids locking the game down, which is super frusterating when throwing an error when in fullscreen mode.
	// This also makes it very convenient when an error is thrown because the game is moved out of the way of the console :)
	window->minimize();
	std::cout << "RUNTIME ERROR: " << err << std::endl << std::endl;
	std::cout << "Press enter to continue execution..." << std::endl;
	// Ignore all characters other than newline
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	window->restore();
}

static bool init()
{
	time = new milk::Time();
	time->delta = TICK_RATE;

	window = new milk::SDLWindow();
	renderer = new milk::SDLRenderer();
	image_cache = new milk::SDLImageCache();
	audio_player = new milk::SDLAudioPlayer();
	music_cache = new milk::SDLMusicCache();
	sound_cache = new milk::SDLSoundCache();
	mouse = new milk::SDLMouse();
	keyboard = new milk::SDLKeyboard();

	milk::Locator::time = time;
	milk::Locator::window = window;
	milk::Locator::renderer = renderer;
	milk::Locator::images = image_cache;
	milk::Locator::audioPlayer = audio_player;
	milk::Locator::sounds = sound_cache;
	milk::Locator::music = music_cache;
	milk::Locator::mouse = mouse;
	milk::Locator::keyboard = keyboard;

	return window->init() // SDL_Init
		&& renderer->init(window->handle())
		&& image_cache->init(renderer->handle()) // IMG_Init
		&& audio_player->init(); // Mix_init
}

static bool init_api_and_callbacks()
{
	lua = luaL_newstate();
	luaL_openlibs(lua);

	// Open all milk libs
	milk::luaM_openlibs(lua);

	// Our error handler is going to be #1 on the stack
	lua_pushcfunction(lua, error_handler);

	if (luaL_loadfile(lua, "main.lua") != LUA_OK || lua_pcall(lua, 0, 1, ERROR_HANDLER_STACK_INDEX) != LUA_OK) {
		const char* stacktrace = lua_tostring(lua, -1);
		print_runtime_error(stacktrace);
		return false;
	}

	// If main.lua fails or does not return a single table, then frig off.
	if (!lua_istable(lua, -1)) {
		print_runtime_error("main.lua must return a table containing callback functions.");
		return false;
	}

	return true;
}

/*
	Run at a fixed timestep of 16 milliseconds.
	This might be considered a 'naive' game loop, but for smaller 2D games, 
	this should not be a problem unless the platform cannot meet the 60 FPS contract.

	We still pass along the delta time to the tick callback, so if the game loop needs to change later,
	we'll already be using it for time dependent calculations in our scripts.
*/
static void main_loop()
{
	window->show();

	double accumulator = TICK_RATE;
	double currentTime = 0;
	double lastTime = 0;

	while (!window->shouldClose()) {
		lastTime = currentTime;
		currentTime = SDL_GetTicks() / 1000.0;
		double frameTime = currentTime - lastTime;

		// If we hit a breakpoint, then we don't want the next frame to be insane in the membrane.
		if (frameTime > 1.0) {
			frameTime = TICK_RATE;
		}

		accumulator += frameTime;

		while (accumulator >= TICK_RATE) {
			mouse->frameBegin();

			SDL_Event event;
			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT) {
					window->close();
				}
				mouse->handleEvent(&event);
			}

			mouse->updateState();
			keyboard->updateState();
			
			lua_getfield(lua, CALLBACK_TABLE_STACK_INDEX, "tick");
			if (lua_pcall(lua, 0, 0, ERROR_HANDLER_STACK_INDEX) != LUA_OK) {
				const char* stacktrace = lua_tostring(lua, -1);
				lua_pop(lua, 1); // Pop stacktrace from stack
				print_runtime_error(stacktrace);
			}

			renderer->clear();

			lua_getfield(lua, CALLBACK_TABLE_STACK_INDEX, "draw");
			if (lua_pcall(lua, 0, 0, ERROR_HANDLER_STACK_INDEX) != LUA_OK) {
				const char* stacktrace = lua_tostring(lua, -1);
				lua_pop(lua, 1); // Pop stacktrace from stack
				print_runtime_error(stacktrace);
			}

			renderer->present();

			time->total += TICK_RATE;
			accumulator -= TICK_RATE;
		}
	}
}

static void deinit()
{
	lua_close(lua);
	free_ptr(keyboard);
	free_ptr(mouse);
	deinit_and_free_ptr(sound_cache);
	deinit_and_free_ptr(music_cache);
	deinit_and_free_ptr(audio_player);
	deinit_and_free_ptr(image_cache);
	deinit_and_free_ptr(renderer);
	deinit_and_free_ptr(window);
	free_ptr(time);

	milk::Locator::time = nullptr;
	milk::Locator::window = nullptr;
	milk::Locator::renderer = nullptr;
	milk::Locator::images = nullptr;
	milk::Locator::audioPlayer = nullptr;
	milk::Locator::sounds = nullptr;
	milk::Locator::music = nullptr;
	milk::Locator::mouse = nullptr;
	milk::Locator::keyboard = nullptr;
}

int milk::run()
{
	if (!init()) {
		print_runtime_error("Error during startup!");
		deinit();
		return MILK_FAIL;
	}
	if (!init_api_and_callbacks()) {
		print_runtime_error("Error during script initialization!");
		deinit();
		return MILK_FAIL;
	}
	main_loop();
	deinit();
	return MILK_SUCCESS;
}
