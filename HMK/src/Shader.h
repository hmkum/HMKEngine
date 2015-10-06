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

    GLuint get_id() const;

private:
    GLuint shader_id_;
};
}
