#include "SceneParser.h"
#include <pugixml/pugixml.hpp>

using namespace hmk;

SceneData SceneParser::scene_data_;

bool SceneParser::parse(const std::string& name)
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(name.c_str());
	if(!result)
	{
		return false;
	}
	pugi::xml_node scene_node = doc.first_child();
	scene_data_.name_ = scene_node.name();

	for(pugi::xml_node atmosphere : scene_node.child("atmosphere"))
	{
		if(atmosphere.name() == std::string("skybox"))
		{
			scene_data_.atmosphere_.skybox_folder_ = atmosphere.attribute("folder").value();
		}
		else
		{
			// TODO_HMK: Implement fog.
		}
	}

	for(pugi::xml_node model : scene_node.children("model"))
	{
		ModelData model_data;
		model_data.file_ = model.attribute("file").value();
		model_data.name_ = model.attribute("name").value();
		
		pugi::xml_node mat_node = model.child("material");
		model_data.material_.roughness_value_ = mat_node.first_child().first_attribute().as_float();
		mat_node = mat_node.first_child().next_sibling();
		model_data.material_.metalness_value_ = mat_node.first_attribute().as_float();

		pugi::xml_node transf_node = model.child("transform");
		pugi::xml_node pos_node = transf_node.child("position");
		pugi::xml_node rot_node = transf_node.child("rotation");
		pugi::xml_node scale_node = transf_node.child("scale");
		model_data.transform_.pos_x_ = pos_node.attribute("x").as_float();
		model_data.transform_.pos_y_ = pos_node.attribute("y").as_float();
		model_data.transform_.pos_z_ = pos_node.attribute("z").as_float();
		model_data.transform_.rot_x_ = rot_node.attribute("x").as_float();
		model_data.transform_.rot_y_ = rot_node.attribute("y").as_float();
		model_data.transform_.rot_z_ = rot_node.attribute("z").as_float();
		model_data.transform_.scale_x_ = scale_node.attribute("x").as_float();
		model_data.transform_.scale_y_ = scale_node.attribute("y").as_float();
		model_data.transform_.scale_z_ = scale_node.attribute("z").as_float();

		scene_data_.model_.push_back(model_data);
	}

	return true;
}