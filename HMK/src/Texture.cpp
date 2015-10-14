#include "Texture.h"
#include "Utility.h"

using namespace hmk;

Texture::Texture()
    : texture_id_(0)
    , width_(256)
    , height_(256)
    , image_component_(3)
{ }

Texture::~Texture()
{
	glDeleteTextures(1, &texture_id_);
}

bool Texture::create(std::string name, int wrapMode, int minFilter, int magFilter, bool genMipMap)
{
	unsigned char *data = stbi_load((TEXTURE_PATH + name).c_str(), &width_, &height_, &image_component_, 0);
	if(data == nullptr)
	{
		HMK_LOG_ERROR("Texture could not load: ", name);
		return false;
	}

	GLint imageFormat = GL_RGB;
	if (image_component_ == 1)
		imageFormat = GL_RED;
	else if (image_component_ == 3)
		imageFormat = GL_RGB;
	else
		imageFormat = GL_RGBA;

	glGenTextures(1, &texture_id_);
	if(texture_id_ == 0)
	{
		HMK_LOG_ERROR("Invalid texture(" + name + ") id");
		return false;
	}
	glBindTexture(GL_TEXTURE_2D, texture_id_);
	glTexImage2D(GL_TEXTURE_2D, 0, imageFormat, width_, height_, 0, imageFormat, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

	if(genMipMap)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);
	return true;
}

bool Texture::bind(int unit)
{
	if (texture_id_ == 0)
	{
		HMK_LOG_ERROR("Texture::bind: Invalid texture id");
		return false;
	}
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, texture_id_);
	
	return true;
}

void Texture::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}