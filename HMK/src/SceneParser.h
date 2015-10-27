#pragma once
#include "glm/glm.hpp"
#include <pugixml/pugixml.hpp>
#include "Utility.h"
#include <fstream>
#include <streambuf>
#include <string>
#include <vector>

namespace hmk
{
struct CameraData
{
	std::string name_;
	std::string projection_;
	glm::vec3 position_, target_, right_, up_;
	float fov_, near_z_, far_z_;
	glm::vec4 ortho_params_;
};

struct FogData
{
	// TODO_HMK: Implement this
};

struct AtmosphereData
{
	std::string skybox_folder_;
	//FogData fog_data_;
};

struct MaterialData
{
	std::string name_;
	float roughness_value_, metalness_value_;
};

struct TransformData
{
	float pos_x_, pos_y_, pos_z_;
	float rot_x_, rot_y_, rot_z_;
	float scale_x_, scale_y_, scale_z_;
};

struct ModelData
{
	std::string name_, file_;
	MaterialData material_;
	TransformData transform_;
};

struct SceneData
{
	std::string name_;
	CameraData camera_;
	AtmosphereData atmosphere_;
	std::vector<ModelData> models_;
};

class SceneParser
{
public:
	static std::string parse(const std::string& name);
	static SceneData get_data() { return scene_data_; }

private:
	static bool parse_camera(const pugi::xml_node& scene_node);
	static bool parse_atmosphere(const pugi::xml_node& scene_node);
	static bool parse_models(const pugi::xml_node& scene_node);
private:
	static SceneData scene_data_;
};
}