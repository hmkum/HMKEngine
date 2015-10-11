#pragma once

#include <GL/gl3w.h>
#include <fstream>
#include <string>
#include <sstream>
#include "Utility.h"

namespace hmk
{
enum class ShaderType: unsigned int {Vertex = 0x8B31, Fragment = 0x8B30};

class Shader
{
public:
    Shader();
	~Shader();

    bool initialize(ShaderType shader_type, std::string shaderName);

	inline GLuint get_id() const { return shader_id_; }

private:
    GLuint shader_id_;
};
}
