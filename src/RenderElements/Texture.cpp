
// Class includes
#include <RenderElements/Texture.h>
#include <glad/glad.h>
#include <Utilities/Logger.h>
#include <Utilities/Defines.h>

// Additional includes
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Static cache of textures
std::unordered_map<std::string, std::shared_ptr<Texture>> Texture::sCache;

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

std::shared_ptr<Texture> Texture::Load(const std::string& _path)
{
	// Checks cache if texture exists
	// returns the texture cache
	auto it = sCache.find(_path);
	if (it != sCache.end())
	{
		//LOG("Texture Load Complete: %s", _path.c_str());
		return it->second;
	}

	//// Extracting the end of the path after the last '/'
	//size_t lastSlashIndex = _path.find_last_of('/');
	//std::string endOfPath = (lastSlashIndex != std::string::npos) ? _path.substr(lastSlashIndex + 1) : _path;

	// otherwise create new texture and assign it to the cache
	std::shared_ptr<Texture> texture = std::make_shared<Texture>(_path);
	sCache[_path] = texture;

	// Log when new texture is created
	LOG("Texture Creation Complete: %s", _path.c_str());
	return texture;
}

std::shared_ptr<Texture> Texture::LoadWhiteTexture()
{
	// loads white texture from file
	std::string path = SOURCE_DIRECTORY("UserAssets/Textures/DefaultTextures/WhiteTexture.jpg");
	//LOG("Texture Load Complete: %s", path.c_str());
	return Load(path);
}

std::shared_ptr<Texture> Texture::LoadBlackTexture()
{
	// loads black texture from file
	std::string path = SOURCE_DIRECTORY("UserAssets/Textures/DefaultTextures/BlackTexture.jpg");
	//LOG("Texture Load Complete: %s", path.c_str());
	return Load(path);
}

std::shared_ptr<Texture> Texture::LoadImage(const std::string& _path)
{
	auto it = sCache.find(_path);
	if (it != sCache.end())
	{
		return it->second;
	}

	// Otherwise create new texture and assign it to the cache
	std::shared_ptr<Texture> texture = std::make_shared<Texture>(_path);
	sCache[_path] = texture;

	// Log when new texture is created
	LOG("Image Creation Complete: %s", _path.c_str());
	return texture;
}

void Texture::LoadTexture()
{
	// gl generate textures
	glGenTextures(1, &mID);
	glBindTexture(GL_TEXTURE_2D, mID);

	// Set texture wrapping/filtering options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//glBindTexture(GL_TEXTURE_2D, mID);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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
		LOG_ERROR("Texture Load Failed: %s", mPath.c_str());
	}
	stbi_image_free(data);
}

void Texture::Unload(const std::string& _path)
{
	// Finds the texture by file path in the cache and removes it from memory
	auto it = sCache.find(_path);
	if(it != sCache.end())
	{
		sCache.erase(it);
	}
	else
		LOG_ERROR("Incorrect file path for unloading of texture");
}

void Texture::ClearCache()
{
	sCache.clear();
}

TextureID Texture::GenSkybox(std::initializer_list<std::string> texturePaths)
{
	TextureID texID;
	// init all textures and assign them to the correct channels
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

	stbi_set_flip_vertically_on_load(false);

	int width, height, nrChannels, index{ 0 };
	for (auto texturePath : texturePaths)
	{
		unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + index++,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
		}
		else
		{
			LOG_ERROR("Cubemap texture failed to load at location: %s", texturePath.c_str());
		}
		stbi_image_free(data);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	return texID;
}
