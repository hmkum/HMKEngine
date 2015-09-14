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

    ShaderProgram &AddShader(const Shader &shader);
    void Link();
    void Use();

	bool SetUniform(std::string name, float f);
	bool SetUniform(std::string name, int i);
	bool SetUniform(std::string name, const glm::vec2 &v);
	bool SetUniform(std::string name, const glm::vec3 &v);
	bool SetUniform(std::string name, const glm::vec4 &v);
	bool SetUniform(std::string name, const glm::ivec4 &v);
	bool SetUniform(std::string name, const glm::mat3 &m);
	bool SetUniform(std::string name, const glm::mat4 &m);

	bool SetUniform(std::string name, const MaterialUniform &mat);

private:
    GLuint mProgramID;
    std::vector<Shader> mShaders;
};
}