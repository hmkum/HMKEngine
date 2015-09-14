#include "ShaderProgram.h"
#include "Utility.h"

using namespace hmk;

ShaderProgram::ShaderProgram()
    : mProgramID(0)
{
}

ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(mProgramID);
}

ShaderProgram &ShaderProgram::AddShader(const Shader &shader)
{
    mShaders.push_back(shader);
    return *this;
}

void ShaderProgram::Link()
{
    mProgramID = glCreateProgram();
    for(auto &shader : mShaders)
    {
        glAttachShader(mProgramID, shader.GetID());
    }
    glLinkProgram(mProgramID);
	glValidateProgram(mProgramID);

    GLint success;
    glGetProgramiv(mProgramID, GL_LINK_STATUS, &success);
    if(!success)
    {
        GLchar log[512];
        glGetProgramInfoLog(mProgramID, 512, nullptr, log);
		HMK_PRINT(log)
		HMK_LOG_ERROR(log)
    }

    for(auto &shader : mShaders)
    {
		glDetachShader(mProgramID, shader.GetID());
        glDeleteShader(shader.GetID());
    }
}

void ShaderProgram::Use()
{
    glUseProgram(mProgramID);
}

bool ShaderProgram::SetUniform(std::string name, float f)
{
	GLint id = glGetUniformLocation(mProgramID, name.c_str());
	if(id == -1)
	{
		HMK_LOG_ERROR("Wrong uniform name(" + name + ")!");
		return false;
	}
	glUniform1f(id,f);

	return true;
}

bool ShaderProgram::SetUniform(std::string name, int i)
{
	GLint id = glGetUniformLocation(mProgramID, name.c_str());
	if(id == -1)
	{
		HMK_LOG_ERROR("Wrong uniform name(" + name + ")!");
		return false;
	}
	glUniform1i(id, i);

	return true;
}

bool ShaderProgram::SetUniform(std::string name, const glm::vec2 &v)
{
	GLint id = glGetUniformLocation(mProgramID, name.c_str());
	if(id == -1)
	{
		HMK_LOG_ERROR("Wrong uniform name(" + name + ")!");
		return false;
	}
	glUniform2f(id, v.x, v.y);

	return true;
}

bool ShaderProgram::SetUniform(std::string name, const glm::vec3 &v)
{
	GLint id = glGetUniformLocation(mProgramID, name.c_str());
	if(id == -1)
	{
		HMK_LOG_ERROR("Wrong uniform name(" + name + ")!");
		return false;
	}
	glUniform3f(id, v.x, v.y, v.z);

	return true;
}

bool ShaderProgram::SetUniform(std::string name, const glm::vec4 &v)
{
	GLint id = glGetUniformLocation(mProgramID, name.c_str());
	if(id == -1)
	{
		HMK_LOG_ERROR("Wrong uniform name(" + name + ")!");
		return false;
	}
	glUniform4f(id, v.x, v.y, v.z, v.w);

	return true;
}

bool ShaderProgram::SetUniform(std::string name, const glm::ivec4 &v)
{
	GLint id = glGetUniformLocation(mProgramID, name.c_str());
	if (id == -1)
	{
		HMK_LOG_ERROR("Wrong uniform name(" + name + ")!");
		return false;
	}
	glUniform4i(id, v.x, v.y, v.z, v.w);

	return true;
}

bool ShaderProgram::SetUniform(std::string name, const glm::mat3 &m)
{
	GLint id = glGetUniformLocation(mProgramID, name.c_str());
	if(id == -1)
	{
		HMK_LOG_ERROR("Wrong uniform name(" + name + ")!");
		return false;
	}
	glUniformMatrix3fv(id, 1, GL_FALSE, &m[0][0]);

	return true;
}


bool ShaderProgram::SetUniform(std::string name, const glm::mat4 &m)
{
	GLint id = glGetUniformLocation(mProgramID, name.c_str());
	if(id == -1)
	{
		HMK_LOG_ERROR("Wrong uniform name(" + name + ")!");
		return false;
	}
	glUniformMatrix4fv(id, 1, GL_FALSE, &m[0][0]);

	return true;
}

bool ShaderProgram::SetUniform(std::string name, const MaterialUniform &mat)
{
	GLint id = glGetUniformLocation(mProgramID, (name + ".BaseColor").c_str());
	if (id == -1)
	{
		HMK_LOG_ERROR("Wrong uniform name(" + name + ".BaseColor)!");
		return false;
	}
	glUniform4f(id, mat.mBaseColor.x, mat.mBaseColor.y, mat.mBaseColor.z, mat.mBaseColor.w);

	id = glGetUniformLocation(mProgramID, (name + ".Roughness").c_str());
	if (id == -1)
	{
		HMK_LOG_ERROR("Wrong uniform name(" + name + ".Roughness)!");
		return false;
	}
	glUniform1f(id, mat.mRoughness);

	id = glGetUniformLocation(mProgramID, (name + ".Metallic").c_str());
	if (id == -1)
	{
		HMK_LOG_ERROR("Wrong uniform name(" + name + ".Metallic)!");
		return false;
	}
	glUniform1f(id, mat.mMetallic);

	return true;
}

