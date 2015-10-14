#pragma once
#include "ShaderProgram.h"
#include <map>
#include <string>
#include <vector>

namespace hmk
{

struct ShaderNames
{
	std::string name_;
	std::string vert_name_;
	std::string frag_name_;
};

struct ShaderWithProgram
{
	std::string name_;
	ShaderProgram program_;
};

class ShaderManager
{
public:
	~ShaderManager();
	ShaderManager(const ShaderManager&) = delete;
	ShaderManager& operator=(const ShaderManager&) = delete;

	static ShaderManager& get_instance();

	void add_shader(const std::string& shader_name);
	void add_shader(const std::string& vert_name, const std::string& frag_name);
	bool compile_and_link_shaders();
	bool use_shader(const std::string& name);
	
	bool set_uniform(std::string name, float f);
	bool set_uniform(std::string name, int i);
	bool set_uniform(std::string name, const glm::vec2 &v);
	bool set_uniform(std::string name, const glm::vec3 &v);
	bool set_uniform(std::string name, const glm::vec4 &v);
	bool set_uniform(std::string name, const glm::ivec4 &v);
	bool set_uniform(std::string name, const glm::mat3 &m);
	bool set_uniform(std::string name, const glm::mat4 &m);

	bool set_uniform(std::string name, const MaterialUniform &mat);

private:
	std::vector<ShaderNames> shader_names_;
	std::vector<ShaderWithProgram> shaders_;
	ShaderProgram current_shader;
private:
	ShaderManager() {}
	std::string get_proper_name(std::string shader_name);
};
}