#ifndef _RESOURCE_MANAGER_
#define _RESOURCE_MANAGER_

#include <unordered_map>
#include <string>

struct SDL_Renderer;

class Texture;

class ResourceManager
{
public:
	explicit ResourceManager(const std::string& rootDir = "");
	~ResourceManager();

	// Initialize the resource manager.
	void init(SDL_Renderer* sdlRenderer);

	// Load texture. Textures are cached.
	Texture* loadTexture(const std::string& name);

	std::string loadFile(const std::string& filename);

	// Unload all loaded textures.
	void unloadTextures();

	// Frees all loaded resources.
	void freeResources();

private:
	std::string rootDir_;

	SDL_Renderer* sdlRenderer_;

	std::unordered_map<std::string, Texture*> textureCache_;
};

#endif
