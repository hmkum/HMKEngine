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
    bool Init(GLenum shaderType, std::string shaderName);

    GLuint GetID() const;

private:
    GLuint mShaderID;
};
}
