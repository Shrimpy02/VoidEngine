#pragma once

// includes
#include <string>
#include <unordered_map>
#include <Utilities/Types.h>

/**
 * @class Texture
 * @brief Manages all textures through the texture cache and
 * creation of new textures using additional helper functions.
 */
class Texture
{
public:
    // ---------- Global Variables --------------

	// texture id used by glfw
    TextureID mID = { 0U };
    // project path to texture location
    std::string mPath; 
    // Static cache of all textures
    static std::unordered_map<std::string, Texture*> sCache;

private:
    // ---------- Local Variables --------------

public:
    // ---------- Global functions --------------

    // Constructor calls load for init.
	Texture(const std::string& _path);

	// Disable move and copy constructors
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&&) = delete;
    Texture& operator=(Texture&&) = delete;

    // deconstructs object and gl texture reference
    ~Texture();

    // Functions ----------- 

    // Loads the texture from the input system path, checks cache if texture exists first,
    // if not creates a new texture and loads it into the cache.
	static Texture* Load(const std::string& _path);

    // Calls Load with default path file to white texture
    static Texture* LoadWhiteTexture();

    // Calls Load with default path file to black texture
    static Texture* LoadBlackTexture();

    // Unloads specific texture from cache by its file path.
    static void Unload(const std::string& _path);

    // Erases all elements of the texture cache
    static void ClearCache();

    // Returns true if mID == 0;
    bool IsValid() const { return mID == 0; }

private:
    // ---------- Local functions --------------

    // Loads local texture into GL using stbi_Image
    void LoadTexture();

public:
    // ---------- Getters / setters / Adders --------------

    // Getters

	// Returns texture ID.
    TextureID GetTextureID() const { return mID; }
};

