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

	bool Load(std::string texturePath);
	void Render();

	GLuint GetTextureID() const;
private:
	GLuint mTextureID;
	GLuint mVAO, mVBO;
};

typedef std::shared_ptr<Skybox> SkyboxPtr;
}