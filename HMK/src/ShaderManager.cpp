#include "ShaderManager.h"
#include <algorithm>
#include <fstream>

using namespace hmk;

ShaderManager& ShaderManager::get_instance()
{
	static ShaderManager instance;
	return instance;
}

ShaderManager::~ShaderManager()
{
	for(auto& shader : shaders_)
	{
		shader.program_.~ShaderProgram();
	}
}

void ShaderManager::add_shader(const std::string& shader_name)
{
	std::string proper_name = get_proper_name(shader_name);
	shader_names_.emplace_back(ShaderNames{proper_name, shader_name, shader_name});
}

void ShaderManager::add_shader(const std::string& vert_name, const std::string& frag_name)
{
	std::string proper_name = get_proper_name(frag_name);
	shader_names_.emplace_back(ShaderNames{proper_name, vert_name, frag_name});
}

bool ShaderManager::compile_and_link_shaders()
{
	bool result = true;
	for(const auto& shader : shader_names_)
	{
		hmk::ShaderProgram shader_program;
		result &= shader_program.add_shader(shader.vert_name_ + ".vert", shader.frag_name_ + ".frag");
		result &= shader_program.link_shaders();
		shaders_.emplace_back(ShaderWithProgram{shader.name_, shader_program});
	}
	return result;
}

bool ShaderManager::use_shader(const std::string& name)
{
	bool result = false;
	for(const auto& shader : shaders_)
	{
		if(shader.name_ == name)
		{
			current_shader = shader.program_;
			current_shader.use();
			result = true;
			break;
		}
	}
	HMK_LOG_WARNING("Could not find shader(" + name + ")!");
	return result;
}

bool ShaderManager::set_uniform(std::string name, float f)
{
	return current_shader.set_uniform(name, f);
}

bool ShaderManager::set_uniform(std::string name, int i)
{
	return current_shader.set_uniform(name, i);
}

bool ShaderManager::set_uniform(std::string name, const glm::vec2 &v)
{
	return current_shader.set_uniform(name, v);
}

bool ShaderManager::set_uniform(std::string name, const glm::vec3 &v)
{
	return current_shader.set_uniform(name, v);
}

bool ShaderManager::set_uniform(std::string name, const glm::vec4 &v)
{
	return current_shader.set_uniform(name, v);
}

bool ShaderManager::set_uniform(std::string name, const glm::ivec4 &v)
{
	return current_shader.set_uniform(name, v);
}

bool ShaderManager::set_uniform(std::string name, const glm::mat3 &m)
{
	return current_shader.set_uniform(name, m);
}


bool ShaderManager::set_uniform(std::string name, const glm::mat4 &m)
{
	return current_shader.set_uniform(name, m);
}

bool ShaderManager::set_uniform(std::string name, const MaterialUniform &mat)
{
	return current_shader.set_uniform(name, mat);
}

std::string ShaderManager::get_proper_name(std::string shader_name)
{
	std::ifstream in(SHADER_PATH + "shader_names.txt");
	if(!in.is_open())
	{
		return std::string("");
	}

	std::string line;
	while(std::getline(in, line))
	{
		auto it = line.find(":");
		std::string proper_name = line.substr(0, it);
		line.erase(0, it + 1);
		if(line.compare(shader_name) == 0)
		{
			return proper_name;
		}
	}

	return shader_name;
}