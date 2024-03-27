#pragma once
#include <Shader.h>
#include <Texture.h>

#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <array>

class Material
{
public:
    // ---------- Global Variables --------------

    enum TextureType
    {
        DIFFUSE,
        SPECULAR,
        NORMAL,
        ALPHA,
        COUNT
    };

    std::array <std::string, static_cast<size_t>(TextureType::COUNT)> TextureUniformNames = {
    	"material.diffuseMap", "material.specularMap",
        "material.normalMap", "material.alphaMap"
    };

    struct MaterialProperties
    {
        glm::vec3 mColor{ 1.f, 1.f, 1.f };
        float mShininess{ 64.f };
    };

private:
    // ---------- Local Variables --------------

    MaterialProperties mProperties{};
    std::array<Texture*, TextureType::COUNT> mTextures{};

    static std::unordered_map<std::string, Material*> sCache;


public:
    // ---------- Global functions --------------
    // Constructors:

    Material(const std::string& _name);

    Material(const Material&) = delete;
    Material& operator=(const Material&) = delete;

    Material(Material&&) = delete;
    Material& operator=(Material&&) = delete;

    // Functions:
    static Material* Load(const std::string& name);
    static Material* Load(const std::string& name, const std::array<Texture*, TextureType::COUNT>& textures, const MaterialProperties& properties);
    static void Unload(const std::string& name);
    static void ClearCache();

    // Assumes the shader is bound
    void Bind(const Shader* shader) const;

    void SetTexture(TextureType type, Texture* texture);
    Texture* GetTexture(TextureType type) const;

    const MaterialProperties& GetProperties() const { return mProperties; }
    void SetProperties(const MaterialProperties& properties) { mProperties = properties; }

private:
    // ---------- Local functions --------------
  


public:
    // ---------- Getters / setters / Adders --------------

    // Adders

    // Setters

    // Getters

};

