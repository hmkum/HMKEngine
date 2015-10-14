#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "Shader.h"
#include "Material.h"

namespace hmk
{
class ShaderProgram
{
public:
    ShaderProgram();
	~ShaderProgram();

	bool add_shader(const std::string &vertex_name, const std::string& fragment_name);
    bool link_shaders();
    void use();

	bool set_uniform(std::string name, float f);
	bool set_uniform(std::string name, int i);
	bool set_uniform(std::string name, const glm::vec2 &v);
	bool set_uniform(std::string name, const glm::vec3 &v);
	bool set_uniform(std::string name, const glm::vec4 &v);
	bool set_uniform(std::string name, const glm::ivec4 &v);
	bool set_uniform(std::string name, const glm::mat3 &m);
	bool set_uniform(std::string name, const glm::mat4 &m);

	bool set_uniform(std::string name, const MaterialUniform &mat);

private:
    GLuint program_id_;
    std::vector<Shader> shaders_;
	std::string shader_names_[2];
};
}