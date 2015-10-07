#pragma once
#include <GL/gl3w.h>
#include <stb/stb_image.h>
#include <string>
#include "Utility.h"

namespace hmk
{
enum class TextureType: unsigned int { Albedo = 0, Normal, Roughness, Metalness};

class Texture
{
public:
	Texture();
	~Texture();

	bool create(std::string name, int wrapMode = GL_REPEAT, int minFilter = GL_LINEAR_MIPMAP_LINEAR, int magFilter = GL_LINEAR, bool genMipMap = true);
	bool bind(int unit = 0);
	void unbind();

	inline void set_texture_type(const TextureType &type) { texture_type_ = type; }
	inline TextureType get_texture_type() const { return texture_type_; }

private:
	GLuint texture_id_;
	int width_, height_, image_component_;
	TextureType texture_type_;
};
}