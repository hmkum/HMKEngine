#include "Shader.h"
#include <iostream>

using namespace hmk;

Shader::Shader()
    : shader_id_(0)
{ }

Shader::~Shader()
{
}

bool Shader::initialize(ShaderType shader_type, std::string shaderName)
{
    std::ifstream file(SHADER_PATH + shaderName);
    if(file.is_open())
    {
		std::string shaderCode;
		std::string line;
        while(std::getline(file, line))
		{
			shaderCode += line + "\n";
		}
		const GLchar *sCode = shaderCode.c_str();
		shader_id_ = glCreateShader((unsigned int)shader_type);
        glShaderSource(shader_id_, 1, &sCode, nullptr);
        glCompileShader(shader_id_);

        GLint success;
        glGetShaderiv(shader_id_, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            GLchar log[512];
            glGetShaderInfoLog(shader_id_, 512, nullptr, log);
			std::cerr << "Shader(" << shaderName << "): " << log << std::endl;
            return false;
        }
        return true;
    }
    else
    {
        return false;
    }
}
