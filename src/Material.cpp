
// Includes
#include "Material.h"
#include "Utilities/Logger.h"

// static cache of materials
std::unordered_map<std::string, Material*> Material::sCache;

Material::Material(const std::string& _name)
{
    // Fill mTextures with nullptrs for defualt init
    mTextures.fill(nullptr);
}

Material* Material::Load(const std::string& _name)
{
    // Calls overload load function
    return Load(_name, {}, {});
}

Material* Material::Load(const std::string& _name, const std::array<Texture*, TextureType::COUNT>& _textures, const MaterialProperties& _properties)
{
    // Checks static cache for object
    auto it = sCache.find(_name);
    if (it != sCache.end())
    {
        // saves processing time by limiting unnecessary material creation.
        LOG("Material Loaded: %s", _name.c_str());
        return sCache[_name];
    }

    // Otherwise create new material and add it to cache. 
    Material* material = new Material(_name);
    material->mTextures = _textures;
    material->mProperties = _properties;
    sCache[_name] = material;
    LOG("Material Created: %s", _name.c_str());
    return material;
}

void Material::Unload(const std::string& _name)
{
    // Finds the materials by name in cache and removes it
    auto it = sCache.find(_name);
    if (it != sCache.end())
    {
        delete it->second;
        sCache.erase(it);
    }
}

void Material::ClearCache()
{
    // clears all elements of cache and removes it from memory
    for (auto it : sCache)
    {
        delete it.second;
    }
    sCache.clear();
}

void Material::Bind(const Shader* _shader) const
{
    if(_shader)
    {
        for (size_t i = 0; i < mTextures.size(); ++i)
        {
            if (mTextures[i])
            {
                // Actives texture
                glActiveTexture(GL_TEXTURE0 + (GLenum) i);
                // binds texture by id
                glBindTexture(GL_TEXTURE_2D, mTextures[i]->GetTextureID());

                // Sets texture to shader
                _shader->setInt(TextureUniformNames[i], static_cast<int>(i));
            }
        }

        // sets material properties to shader
        _shader->setVec3("material.diffuseColor", mProperties.mColor);
        _shader->setFloat("material.shininess", mProperties.mShininess);
    } else
        LOG_ERROR("Shader not found when binding material");
}

void Material::SetTexture(TextureType _type, Texture* _texture)
{
    // Finds the relevant texture type and replaces its index with the input texture pointer.
    if (_type >= 0 && _type < TextureType::COUNT)
    {
        mTextures[_type] = _texture;
    }
}

Texture* Material::GetTexture(TextureType _type) const
{
    // Finds the relevant texture type and returns it. 
    if (_type >= 0 && _type < TextureType::COUNT)
    {
        return mTextures[_type];
    }
    // if incorrect texture type return null pointer;
    LOG_WARNING("Incorrect texture type passed for get function");
    return nullptr;
}

Material* Material::GetMaterialFromCache(std::string& _name)
{
    // finds material in cache by name and returns it. 
    auto it = sCache.find(_name);
    if (it != sCache.end())
        return it->second;

    // if incorrect name or its not in the cache print error and return nullptr
	LOG_ERROR("Material %s, not found in cache", _name);
    return nullptr;
}
