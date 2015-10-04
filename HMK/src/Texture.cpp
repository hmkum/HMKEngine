#include "Texture.h"
#include "Utility.h"

using namespace hmk;

Texture::Texture()
    : mTextureID(0)
    , mWidth(256)
    , mHeight(256)
    , mComponent(3)
{ }

Texture::~Texture()
{
	glDeleteTextures(1, &mTextureID);
}

bool Texture::Create(std::string name, int wrapMode, int minFilter, int magFilter, bool genMipMap)
{
	unsigned char *data = stbi_load((TEXTURE_PATH + name).c_str(), &mWidth, &mHeight, &mComponent, 0);
	if(data == nullptr)
	{
		HMK_LOG_ERROR("Texture could not load: ", name);
		return false;
	}

	GLint imageFormat = GL_RGB;
	if (mComponent == 1)
		imageFormat = GL_RED;
	else if (mComponent == 3)
		imageFormat = GL_RGB;
	else
		imageFormat = GL_RGBA;

	glGenTextures(1, &mTextureID);
	if(mTextureID == 0)
	{
		HMK_LOG_ERROR("Invalid texture(" + name + ") id");
		return false;
	}
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, imageFormat, mWidth, mHeight, 0, imageFormat, GL_UNSIGNED_BYTE, data);

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

bool Texture::Bind(int unit)
{
	if (mTextureID == 0)
	{
		HMK_LOG_ERROR("Texture::Bind: Invalid texture id");
		return false;
	}
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	
	return true;
}

void Texture::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}