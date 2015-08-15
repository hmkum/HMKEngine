#pragma once
#include <GL/gl3w.h>
#include <stb/stb_image.h>
#include <string>
#include "Utility.h"

namespace hmk
{
class Texture
{
public:
        Texture();
	bool Create(std::string name, int wrapMode = GL_REPEAT, int minFilter = GL_LINEAR_MIPMAP_LINEAR, int magFilter = GL_LINEAR, bool genMipMap = true);
	bool Bind(int unit = 0);
	void Unbind();
private:
	GLuint mTextureID;
	int mWidth, mHeight, mComponent;
};
}