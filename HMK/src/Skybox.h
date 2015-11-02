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

	inline std::string get_folder_name() const { return folder_name_; }
	inline GLuint get_texture_id() const { return texture_id_; }
private:
	std::string folder_name_;
	GLuint texture_id_;
	GLuint vao_id_, vbo_id_;
};

using SkyboxPtr = std::shared_ptr<Skybox>; // TODO_HMK: Delete this
using SkyboxSPtr = std::shared_ptr<Skybox>;
using SkyboxUPtr = std::unique_ptr<Skybox>;
}