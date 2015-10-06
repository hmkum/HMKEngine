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
	~Texture();

	bool create(std::string name, int wrapMode = GL_REPEAT, int minFilter = GL_LINEAR_MIPMAP_LINEAR, int magFilter = GL_LINEAR, bool genMipMap = true);
	bool bind(int unit = 0);
	void unbind();
private:
	GLuint texture_id_;
	int width_, height_, image_component_;
};
}