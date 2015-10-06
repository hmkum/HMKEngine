#pragma once
#include <GL/gl3w.h>
#include <stb/stb_image.h>
#include "Utility.h"

#include <memory>
#include <string>
#include <vector>

namespace hmk
{
class Skybox
{
public:
	Skybox();
	~Skybox();

	bool load(std::string texturePath);
	void render();

	GLuint get_texture_id() const;
private:
	GLuint texture_id_;
	GLuint vao_id_, vbo_id_;
};

typedef std::shared_ptr<Skybox> SkyboxPtr;
}