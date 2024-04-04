
// Class includes
#include <Texture.h>
#include <Logger.h>
#include <glad/glad.h>
#include <Defines.h>

// Additional includes
// STBI
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Static cache of textures
std::unordered_map<std::string, Texture*> Texture::sCache;

Texture::Texture(const std::string& _path) : mID(0), mPath(_path)
{
	// stbi loads texture for gl using path
	LoadTexture();
}

Texture::~Texture()
{
	// deletes texture from gl memory
	glDeleteTextures(1,&mID);
}

Texture* Texture::Load(const std::string& _path)
{
	// Checks cache if texture exists
	// returns the texture cache
	auto it = sCache.find(_path);
	if (it != sCache.end())
		return it->second;

	// Extracting the end of the path after the last '/'
	size_t lastSlashIndex = _path.find_last_of('/');
	std::string endOfPath = (lastSlashIndex != std::string::npos) ? _path.substr(lastSlashIndex + 1) : _path;

	// otherwise create new texture and assign it to the cache
	Texture* texture = new Texture(_path);
	sCache[_path] = texture;
	LOG("Texture Load Complete: %s", endOfPath.c_str());
	return texture;
}

Texture* Texture::LoadWhiteTexture()
{
	// loads white texture from file
	return Load(SOURCE_DIRECTORY("assets/Textures/DefaultTextures/WhiteTexture.jpg"));
}

Texture* Texture::LoadBlackTexture()
{
	// loads black texture from file
	return Load(SOURCE_DIRECTORY("assets/Textures/DefaultTextures/BlackTexture.jpg"));
}

void Texture::LoadTexture()
{
	// gl generate textures
	glGenTextures(1, &mID);
	glBindTexture(GL_TEXTURE_2D, mID);

	// stbi init textures
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
	// Finds the texture by file path in the cache and removes it from memory
	auto it = sCache.find(_path);
	if(it != sCache.end())
	{
		delete it->second;
		sCache.erase(it);
	}
	else
		LOG_ERROR("Incorrect file path for unloading of texture");
	
}

void Texture::ClearCache()
{
	// deletes all elements of the cache
	for (auto it : sCache)
	{
		delete it.second;
	}
	sCache.clear();
}