#ifndef MILK_GAME_H
#define MILK_GAME_H

// Exit codes
#define MILK_SUCCESS 0
#define MILK_FAIL 1

#include <memory>
#include <stack>
#include <string>
#include <unordered_map>

#include "sol/sol.hpp"
#include "json/json.hpp"

namespace milk
{
    template<class T>
    class AssetCache;

#ifdef _DEBUG
    class DebugTools;
#endif

    class Filesystem;
    class GameState;
    class Texture;
    class Window;

    namespace adapter
    {
        class ActorTemplateCacheAdapter;
        class FilesystemAdapter;
        class TextureCacheAdapter;
        class WindowAdapter;
    }

    /// The Game is in charge of startup, shutdown, state and scene management.
    class Game
    {
    public:
        /// \returns the single instance of the Game.
        static Game& getInstance()
        {
            static Game instance;
            return instance;
        }

        ~Game();

        /// Initializes the Game and its subsystems.
        /// \param configFilepath: The path to the Game's configuration file.
        void init(std::string configFilepath);

        /// Initializes and runs the game
        /// Returns MILK_SUCCESS on successful run
        /// Returns MILK_FAIL on unsuccessful run
        int run();

        /// Changes the Game's current state.
        /// \param state: The new state of the Game.
        void changeState(std::unique_ptr<GameState> state);

        /// Pushes a GameState onto the top of the GameState stack.
        /// \param state: The state to push onto the Game's state stack.
        void pushState(std::unique_ptr<GameState> state);

        /// Pops a GameState off of the game state stack.
        void popState();

        /// \returns the game window.
        Window& window() const;

        /// \returns the games filesystem.
        Filesystem& filesystem() const;

        /// \returns the games texture cache.
        AssetCache<Texture>& textureCache() const;

        /// \returns the game actor template cache.
        AssetCache<nlohmann::json>& actorTemplateCache() const;

        /// \returns the games lua state.
        sol::state& luaState();

#ifdef _DEBUG
        /// \returns the Game's debug tools if in debug mode.
        DebugTools& debugTools() const;
#endif

        /// Attempts to load the requested Scene.
        /// \param name
        void loadScene(const std::string& name);

        /// Quits and shuts down the Game.
        void quit();

    private:
        /// \param configFile: Path to the config file
        Game();

        std::stack<std::unique_ptr<GameState>> stateStack_;

        std::string configFile_;

        std::unique_ptr<adapter::WindowAdapter> window_;
        std::unique_ptr<adapter::FilesystemAdapter> fileSystem_;
        std::unique_ptr<adapter::TextureCacheAdapter> textureCache_;
        std::unique_ptr<adapter::ActorTemplateCacheAdapter> actorTemplateCache_;

        sol::state luaState_;

#ifdef _DEBUG
        std::unique_ptr<DebugTools> debugTools_;
#endif

        bool initialized_;
        bool isRunning_;

        void handleEvents();
        void update();
        void render();
        void shutDown();
    };
}

#endif
