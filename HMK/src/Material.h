#pragma once
#include <glm/glm.hpp>
#include <string>

namespace hmk
{

struct MaterialUniform
{
	MaterialUniform()
		: base_color_{glm::vec4(0.0f)}
		, roughness_{0.0f}
		, metallic_{0.0f}
	{}

	glm::vec4 base_color_;
	float roughness_;
	float metallic_;
};

struct Material
{
	Material()
		: name_{"DefaultMat"}
		, base_color_{glm::vec3(1.0f)}
		, roughness_{0.04f}
		, metallic_{0.0f}
		, has_textures_{glm::ivec4(0)}
	{}
	std::string name_;
	
	glm::vec3 base_color_;
	float roughness_;
	float metallic_;
	
	std::string albedo_texture_name_;
	std::string normal_texture_name_;
	std::string roughness_texture_name_;
	std::string metallic_texture_name_;	
	
	glm::ivec4 has_textures_; // x: HasAlbedo?, y: HasNormal, z: HasRoughness, w: HasMetallic;
};
}