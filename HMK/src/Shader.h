#pragma once

#include <GL/gl3w.h>
#include <fstream>
#include <string>
#include <sstream>
#include "Utility.h"

namespace hmk
{
class Shader
{
public:
    Shader();
	~Shader();

    bool initialize(GLenum shaderType, std::string shaderName);

	inline GLuint get_id() const { return shader_id_; }

private:
    GLuint shader_id_;
};
}
