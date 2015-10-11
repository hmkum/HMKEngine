#pragma once
#include "Utility.h"
#include <fstream>
#include <streambuf>
#include <string>
#include <vector>
#include <rapidxml/rapidxml.hpp>

namespace hmk
{
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