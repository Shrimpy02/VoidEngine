#include "Material.h"

std::unordered_map<std::string, Material*> Material::sCache;

Material::Material(const std::string& _name)
{
    mTextures.fill(nullptr);
}

Material* Material::Load(const std::string& _name)
{
    return Load(_name, {}, {});
}

Material* Material::Load(const std::string& _name, const std::array<Texture*, TextureType::COUNT>& _textures, const MaterialProperties& _properties)
{
   // auto it = sCache.find(_name);
   // if (it != sCache.end())
   // {
   //     return sCache[_name];
   // }

    Material* material = new Material(_name);
    material->mTextures = _textures;
    material->mProperties = _properties;
    sCache[_name] = material;
    return material;
}

void Material::Unload(const std::string& name)
{
    auto it = sCache.find(name);
    if (it != sCache.end())
    {
        delete it->second;
        sCache.erase(it);
    }
}

void Material::ClearCache()
{
    for (auto it : sCache)
    {
        delete it.second;
    }
    sCache.clear();
}

void Material::Bind(const Shader* _shader) const
{
    for (size_t i = 0; i < mTextures.size(); ++i)
    {
        if (mTextures[i])
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, mTextures[i]->GetTextureID());

            _shader->setInt(TextureUniformNames[i], static_cast<int>(i));
        }
    }

    _shader->setVec3("material.diffuseColor", mProperties.mColor);
    _shader->setFloat("material.shininess", mProperties.mShininess);
}

void Material::SetTexture(TextureType _type, Texture* _texture)
{
    if (_type >= 0 && _type < TextureType::COUNT)
    {
        mTextures[_type] = _texture;
    }
}

Texture* Material::GetTexture(TextureType _type) const
{
    if (_type >= 0 && _type < TextureType::COUNT)
    {
        return mTextures[_type];
    }
    return nullptr;
}