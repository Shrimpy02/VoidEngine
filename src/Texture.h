#pragma once

#include <glad/glad.h>
#include <string>
#include <unordered_map>
#include <Types.h>

class Texture
{
public:
    // ---------- Global Variables --------------
    TextureID mID = { 0U };
    std::string mPath; 

    static std::unordered_map<std::string, Texture*> sCache;

private:
    // ---------- Local Variables --------------



public:
    // ---------- Global functions --------------
    // Constructors:
	Texture(const std::string& _path);
    ~Texture();

    // Disable move and copy constructors
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&&) = delete;
    Texture& operator=(Texture&&) = delete;

    // Functions:
	static Texture* Load(const std::string& _path);
    static Texture* LoadWhiteTexture();
    static Texture* LoadBlackTexture();
    static void Unload(const std::string& _path);
    static void ClearCache();

    bool IsValid() const { return mID == 0; }
    TextureID GetTextureID() const { return mID; }

private:
    // ---------- Local functions --------------
    void LoadTexture();


public:
    // ---------- Getters / setters / Adders --------------

    // Adders

    // Setters

    // Getters

};

