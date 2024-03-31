#include <Texture.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Defines.h"

std::unordered_map<std::string, Texture*> Texture::sCache;

Texture::Texture(const std::string& _path) : mID(0), mPath(_path)
{
	LoadTexture();
}

Texture::~Texture()
{
	glDeleteTextures(1,&mID);
}

Texture* Texture::Load(const std::string& _path)
{
	auto it = sCache.find(_path);
	if (it != sCache.end())
	{
		return it->second;
	}
	else
	{
		Texture* texture = new Texture(_path);
		sCache[_path] = texture;
		return texture;
	}
}

Texture* Texture::LoadWhiteTexture()
{
	return Load(SOURCE_DIRECTORY("assets/Textures/DefaultTextures/WhiteTexture.jpg"));
}

Texture* Texture::LoadBlackTexture()
{
	return Load(SOURCE_DIRECTORY("assets/Textures/DefaultTextures/BlackTexture.jpg"));
}

void Texture::LoadTexture()
{
	glGenTextures(1, &mID);
	glBindTexture(GL_TEXTURE_2D, mID);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(mPath.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		GLenum format = GL_RGB;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 4)
			format = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
	{
		std::cout << "Texture Load Failed\n";
		std::cout << mPath.c_str();
	}
	stbi_image_free(data);
}

void Texture::Unload(const std::string& _path)
{
	auto it = sCache.find(_path);
	if(it != sCache.end())
	{
		delete it->second;
		sCache.erase(it);
	}
	else
	{
		std::cout << "Texture not found in chache for deleteion\n";
	}
}

void Texture::ClearCache()
{
	for (auto it : sCache)
	{
		delete it.second;
	}
	sCache.clear();
}