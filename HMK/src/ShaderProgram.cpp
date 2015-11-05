#include "ShaderProgram.h"
#include "Utility.h"

using namespace hmk;

ShaderProgram::ShaderProgram()
    : program_id_(0)
{ }

ShaderProgram::~ShaderProgram()
{
}

bool ShaderProgram::add_shader(const std::string &vertex_name, const std::string& fragment_name)
{
	Shader vert_shader, frag_shader;
	bool result = vert_shader.initialize(ShaderType::Vertex, vertex_name);
	if(!result)
	{
		HMK_PRINT("Could not load shader(" + vertex_name + ")!");
		return false;
	}
	result = frag_shader.initialize(ShaderType::Fragment, fragment_name);
	if(!result)
	{
		HMK_PRINT("Could not load shader(" + fragment_name + ")!");
		return false;
	}

	shader_names_[0] = vertex_name;
	shader_names_[1] = fragment_name;
	shaders_.push_back(vert_shader);
	shaders_.push_back(frag_shader);
	return true;
}

bool ShaderProgram::link_shaders()
{
    program_id_ = glCreateProgram();
    for(auto &shader : shaders_)
    {
        glAttachShader(program_id_, shader.get_id());
    }
    glLinkProgram(program_id_);

    GLint success;
    glGetProgramiv(program_id_, GL_LINK_STATUS, &success);
    if(!success)
    {
        GLchar log[512];
        glGetProgramInfoLog(program_id_, 512, nullptr, log);
		HMK_PRINT(log)
		return false;
    }

	glValidateProgram(program_id_);
	glGetProgramiv(program_id_, GL_VALIDATE_STATUS, &success);
	if(!success)
	{
		GLchar log[512];
		glGetProgramInfoLog(program_id_, 512, nullptr, log);
		HMK_PRINT(log)
		return false;
	}

    for(auto &shader : shaders_)
    {
		glDetachShader(program_id_, shader.get_id());
        glDeleteShader(shader.get_id());
    }

	return true;
}

void ShaderProgram::use()
{
    glUseProgram(program_id_);
}

bool ShaderProgram::set_uniform(std::string name, float f)
{
	GLint id = glGetUniformLocation(program_id_, name.c_str());
	if(id == -1)
	{
		HMK_PRINT("Wrong uniform name(" + name + ")!Program Id :" + std::to_string(program_id_) + " Shader names: " + shader_names_[0] + ", " + shader_names_[1]);
		return false;
	}
	glUniform1f(id,f);

	return true;
}

bool ShaderProgram::set_uniform(std::string name, int i)
{
	GLint id = glGetUniformLocation(program_id_, name.c_str());
	if(id == -1)
	{
		HMK_PRINT("Wrong uniform name(" + name + ")! Program Id: " + std::to_string(program_id_) + " Shader names: " + shader_names_[0] + ", " + shader_names_[1]);
		return false;
	}
	glUniform1i(id, i);

	return true;
}

bool ShaderProgram::set_uniform(std::string name, const glm::vec2 &v)
{
	GLint id = glGetUniformLocation(program_id_, name.c_str());
	if(id == -1)
	{
		HMK_PRINT("Wrong uniform name(" + name + ")! Program Id: " + std::to_string(program_id_) + " Shader names: " + shader_names_[0] + ", " + shader_names_[1]);
		return false;
	}
	glUniform2f(id, v.x, v.y);

	return true;
}

bool ShaderProgram::set_uniform(std::string name, const glm::vec3 &v)
{
	GLint id = glGetUniformLocation(program_id_, name.c_str());
	if(id == -1)
	{
		HMK_PRINT("Wrong uniform name(" + name + ")! Program Id: " + std::to_string(program_id_) + " Shader names: " + shader_names_[0] + ", " + shader_names_[1]);
		return false;
	}
	glUniform3f(id, v.x, v.y, v.z);

	return true;
}

bool ShaderProgram::set_uniform(std::string name, const glm::vec4 &v)
{
	GLint id = glGetUniformLocation(program_id_, name.c_str());
	if(id == -1)
	{
		HMK_PRINT("Wrong uniform name(" + name + ")! Program Id: " + std::to_string(program_id_) + " Shader names: " + shader_names_[0] + ", " + shader_names_[1]);
		return false;
	}
	glUniform4f(id, v.x, v.y, v.z, v.w);

	return true;
}

bool ShaderProgram::set_uniform(std::string name, const glm::ivec4 &v)
{
	GLint id = glGetUniformLocation(program_id_, name.c_str());
	if (id == -1)
	{
		HMK_PRINT("Wrong uniform name(" + name + ")! Program Id: " + std::to_string(program_id_) + " Shader names: " + shader_names_[0] + ", " + shader_names_[1]);
		return false;
	}
	glUniform4i(id, v.x, v.y, v.z, v.w);

	return true;
}

bool ShaderProgram::set_uniform(std::string name, const glm::mat3 &m)
{
	GLint id = glGetUniformLocation(program_id_, name.c_str());
	if(id == -1)
	{
		HMK_PRINT("Wrong uniform name(" + name + ")! Program Id: " + std::to_string(program_id_) + " Shader names: " + shader_names_[0] + ", " + shader_names_[1]);
		return false;
	}
	glUniformMatrix3fv(id, 1, GL_FALSE, &m[0][0]);

	return true;
}


bool ShaderProgram::set_uniform(std::string name, const glm::mat4 &m)
{
	GLint id = glGetUniformLocation(program_id_, name.c_str());
	if(id == -1)
	{
		HMK_PRINT("Wrong uniform name(" + name + ")! Program Id: " + std::to_string(program_id_) + " Shader names: " + shader_names_[0] + ", " + shader_names_[1]);
		return false;
	}
	glUniformMatrix4fv(id, 1, GL_FALSE, &m[0][0]);

	return true;
}

bool ShaderProgram::set_uniform(std::string name, const MaterialUniform &mat)
{
	GLint id = glGetUniformLocation(program_id_, (name + ".BaseColor").c_str());
	if (id == -1)
	{
		HMK_PRINT("Wrong uniform name(" + name + ".BaseColor)!");
		return false;
	}
	glUniform4f(id, mat.base_color_.x, mat.base_color_.y, mat.base_color_.z, mat.base_color_.w);

	id = glGetUniformLocation(program_id_, (name + ".Roughness").c_str());
	if (id == -1)
	{
		HMK_PRINT("Wrong uniform name(" + name + ".Roughness)!");
		return false;
	}
	glUniform1f(id, mat.roughness_);

	id = glGetUniformLocation(program_id_, (name + ".Metallic").c_str());
	if (id == -1)
	{
		HMK_PRINT("Wrong uniform name(" + name + ".Metallic)!");
		return false;
	}
	glUniform1f(id, mat.metallic_);

	return true;
}

bool ShaderProgram::set_uniform(std::string name, const LightUniform light[8], int active_light_count)
{
	for(int i = 0; i < active_light_count; ++i)
	{
		std::string ss = name + "[" + std::to_string(i) + "].Position";
		GLint id = glGetUniformLocation(program_id_, (name + "[" + std::to_string(i) + "].Position").c_str());
		if(id == -1)
		{
			HMK_PRINT("Wrong uniform name(" + name + "[" + std::to_string(i) + "].Position)!");
			return false;
		}
		glUniform3f(id, light[i].position_.x, light[i].position_.y, light[i].position_.z);

		id = glGetUniformLocation(program_id_, (name + "[" + std::to_string(i) + "].Target").c_str());
		if(id == -1)
		{
			HMK_PRINT("Wrong uniform name(" + name + "[" + std::to_string(i) + "].Target)!");
			return false;
		}
		glUniform3f(id, light[i].target_.x, light[i].target_.y, light[i].target_.z);

		id = glGetUniformLocation(program_id_, (name + "[" + std::to_string(i) + "].Color").c_str());
		if(id == -1)
		{
			HMK_PRINT("Wrong uniform name(" + name + "[" + std::to_string(i) + "].Color)!");
			return false;
		}
		glUniform4f(id, light[i].color_.x, light[i].color_.y, light[i].color_.z, light[i].color_.w);

		/*id = glGetUniformLocation(program_id_, (name + "[" + std::to_string(i) + "].Attenuation").c_str());
		if(id == -1)
		{
			HMK_PRINT("Wrong uniform name(" + name + "[" + std::to_string(i) + "].Attenuation)!");
			return false;
		}
		glUniform4f(id, light[i].attenuation_.x, light[i].attenuation_.y, light[i].attenuation_.z, light[i].attenuation_.w);

		id = glGetUniformLocation(program_id_, (name + "[" + std::to_string(i) + "].SpotLightAngle").c_str());
		if(id == -1)
		{
			HMK_PRINT("Wrong uniform name(" + name + "[" + std::to_string(i) + "].SpotLightAngle)!");
			return false;
		}
		glUniform1f(id, light[i].spot_light_angle_);*/

		id = glGetUniformLocation(program_id_, (name + "[" + std::to_string(i) + "].LightType").c_str());
		if(id == -1)
		{
			HMK_PRINT("Wrong uniform name(" + name + "[" + std::to_string(i) + "].LightType)!");
			return false;
		}
		glUniform1i(id, light[i].light_type_);
	}
	return true;
}