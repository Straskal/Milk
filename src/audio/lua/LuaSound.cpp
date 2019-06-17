#include "LuaSound.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
}

#include "audio/Sound.h"
#include "audio/SoundCache.h"
#include "core/Locator.h"

static const char* METATABLE = "milk.soundhandle";

static int new_sound(lua_State* L) {
	if (lua_isstring(L, 1)) {
		const char* value = lua_tostring(L, 1);
		milk::Sound* sound = milk::Locator::sounds->load(value);
		if (sound != nullptr) {
			milk::SoundHandle* handle = (milk::SoundHandle*)lua_newuserdata(L, sizeof(milk::SoundHandle*));
			luaL_getmetatable(L, METATABLE);
			lua_setmetatable(L, -2);
			handle->sound = sound;
			handle->channel = -1;
			lua_pushboolean(L, true);
			return 2;
		}
	}
	lua_pushnil(L);
	lua_pushboolean(L, false);
	return 2;
}

static const luaL_Reg funcs[] = {
	{ "new", new_sound },
	{ NULL, NULL }
};

static int gc(lua_State* L) {
	milk::SoundHandle* handle = (milk::SoundHandle*)luaL_checkudata(L, 1, METATABLE);
	// Stop sound before derefencing it. We do NOT want to release a sound from memory while it is playing.
	milk::Locator::audioPlayer->stopSound(handle);
	milk::Locator::sounds->dereference(handle->sound);
	return 0;
}

static int play(lua_State* L) {
	milk::SoundHandle* handle = (milk::SoundHandle*)luaL_checkudata(L, 1, METATABLE);
	milk::Locator::audioPlayer->playSound(handle);
	return 0;
}

static int stop(lua_State* L) {
	milk::SoundHandle* handle = (milk::SoundHandle*)luaL_checkudata(L, 1, METATABLE);
	milk::Locator::audioPlayer->stopSound(handle);
	return 0;
}

static const luaL_Reg meta_funcs[] = {
	{ "__gc", gc },
	{ "play", play },
	{ "stop", stop },
	{ NULL, NULL }
};

int milk::luaopen_sound(lua_State* L) {
	luaL_newmetatable(L, METATABLE);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, meta_funcs, 0);
	lua_pop(L, 1); // Pop metatable off of stack
	luaL_newlib(L, funcs);
	return 1;
}
