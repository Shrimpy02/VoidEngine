#pragma once
// Includes
#include <Core/Shader.h>

// Additional Includes
#include <memory> 
#include <array>
#include <unordered_map>

// Forward Declarations
class Texture;

/**
 * @class Material
 * @brief Contains textures and properties for mesh objects to use.
 * Has helper functions for managing and uses a static cache for managing all materials in application.
 */
class Material
{
public:
    // ---------- Global Variables --------------

    // Enum for texture type definition
    enum TextureType
    {
        DIFFUSE,
        SPECULAR,
        NORMAL,
        ALPHA,
        COUNT
    };

    // Array of material map names for assigning to shader uniform
    std::array<std::string, static_cast<size_t>(TextureType::COUNT)> TextureUniformNames = {
     "material.diffuseMap", "material.specularMap",
     "material.normalMap", "material.alphaMap"
    };

    // Properties of materials
    struct MaterialProperties
    {
        glm::vec3 mColor{ 1.f, 1.f, 1.f };
        float mShininess{ 64.f };
    };

private:
    // ---------- Local Variables --------------

    MaterialProperties mProperties{};
    std::array<std::shared_ptr<Texture>, TextureType::COUNT> mTextures{};

    // Static Cache of materials
    static std::unordered_map<std::string, std::shared_ptr<Material>> sCache;

public:
    // ---------- Global functions --------------
    // Constructor, initializes mTextures to all nullptr`s
    Material(const std::string& _name);

    // Remove copy and move functionality
    Material(const Material&) = delete;
    Material& operator=(const Material&) = delete;
    Material(Material&&) = delete;
    Material& operator=(Material&&) = delete;

    // Functions

	// Calls the overloaded Load function with default values
    static std::shared_ptr<Material> Load(const std::string& _name);

    // Overload of load function, checks cache if material exists already, if not create new material, return it and add it to cache.
    static std::shared_ptr<Material> Load(const std::string& _name, const std::array<std::shared_ptr<Texture>, TextureType::COUNT>& _textures, const MaterialProperties& _properties);

    // Removes material from cache by name
	static void Unload(const std::string& _name);

    // Removes all materials from cache
    static void ClearCache();

    // Assuming shader is bound, set`s the array of texture names as shader uniforms
    // with accompanying texture maps. Also assigns material properties through uniform to shader. 
    void Bind(const std::shared_ptr<Shader> _shader) const;

private:
    // ---------- Local functions --------------

public:
    // ---------- Getters / setters / Adders --------------

    // Getters

	// Gets the texture based on type from this material. 
    std::shared_ptr<Texture> GetTexture(TextureType _type) const;

    // Finds a material in the cache by name and returns it.
    static std::shared_ptr<Material> GetMaterialFromCache(std::string& _name);

    // Gets the material properties for this material. 
    const MaterialProperties& GetProperties() const { return mProperties; }

    // Setters

    // Set the input texture of input type to the assigned place in the texture array.
    void SetTexture(TextureType _type, std::shared_ptr<Texture> _texture);

    // Sets the material properties from input.
    void SetProperties(const MaterialProperties& _properties) { mProperties = _properties; }


};

