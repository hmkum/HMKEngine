#pragma once
#include "Utility.h"
#include <fstream>
#include <streambuf>
#include <string>
#include <vector>

namespace hmk
{
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
	AtmosphereData atmosphere_;
	std::vector<ModelData> model_;
};

class SceneParser
{
public:
	static bool parse(const std::string& name);
	static SceneData get_data() { return scene_data_; }

private:
	static SceneData scene_data_;
};
}