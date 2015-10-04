#include "Shader.h"
#include <iostream>

using namespace hmk;

Shader::Shader()
    : mShaderID(0)
{ }

Shader::~Shader()
{
}

bool Shader::Init(GLenum shaderType, std::string shaderName)
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
        mShaderID = glCreateShader(shaderType);
        glShaderSource(mShaderID, 1, &sCode, nullptr);
        glCompileShader(mShaderID);

        GLint success;
        glGetShaderiv(mShaderID, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            GLchar log[512];
            glGetShaderInfoLog(mShaderID, 512, nullptr, log);
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

GLuint Shader::GetID() const
{
    return mShaderID;
}
