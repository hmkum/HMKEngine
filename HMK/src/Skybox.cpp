#include "Skybox.h"

using namespace hmk;

Skybox::Skybox()
	: texture_id_{0}
	, vao_id_{0}
	, vbo_id_{0}
{ }

Skybox::~Skybox()
{
	glDeleteTextures(1, &texture_id_);
	glDeleteBuffers(1, &vbo_id_);
	glDeleteVertexArrays(1, &vao_id_);
}

bool Skybox::load(std::string texturePath)
{
	glGenTextures(1, &texture_id_);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id_);

	std::vector<std::string> textureNames;
	textureNames.push_back(SKYBOX_TEXTURE_PATH + texturePath + "posx.jpg");
	textureNames.push_back(SKYBOX_TEXTURE_PATH + texturePath + "negx.jpg");
	textureNames.push_back(SKYBOX_TEXTURE_PATH + texturePath + "posy.jpg");
	textureNames.push_back(SKYBOX_TEXTURE_PATH + texturePath + "negy.jpg");
	textureNames.push_back(SKYBOX_TEXTURE_PATH + texturePath + "posz.jpg");
	textureNames.push_back(SKYBOX_TEXTURE_PATH + texturePath + "negz.jpg");

	for(size_t i = 0; i < textureNames.size(); ++i)
	{
		int x, y, comp;
		unsigned char* data = stbi_load(textureNames[i].c_str(), &x, &y, &comp, 0);
		if (data == nullptr)
		{
			HMK_LOG_ERROR("Texture could not load: ", textureNames[i]);
			return false;
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);


	GLfloat skyboxVertices[] = {
		// Positions          
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f
	};

	glGenVertexArrays(1, &vao_id_);
	glBindVertexArray(vao_id_);

	glGenBuffers(1, &vbo_id_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_id_);

	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)* 3, (GLvoid*)0);
	glBindVertexArray(0);

	return true;
}

void Skybox::render()
{
	glDepthMask(GL_FALSE);
	glFrontFace(GL_CCW);
	glBindVertexArray(vao_id_);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id_);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glBindVertexArray(0);
	glFrontFace(GL_CW);
	glDepthMask(GL_TRUE);
}

GLuint Skybox::get_texture_id() const
{
	return texture_id_;
}