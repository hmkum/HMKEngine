#include "SceneParser.h"

using namespace hmk;

SceneData SceneParser::scene_data_;

bool SceneParser::parse(const std::string& name)
{
	std::ifstream scene_file(name);
	if(!scene_file.is_open())
	{
		HMK_LOG_ERROR("Could not load scene.xml file!");
		return false;
	}

	std::string scene_text;
	scene_text.assign(std::istreambuf_iterator<char>(scene_file), std::istreambuf_iterator<char>());
	scene_file.close();

	rapidxml::xml_document<> xml_doc;
	xml_doc.parse<0>(&scene_text[0]);

	rapidxml::xml_node<> *main_node = xml_doc.first_node("scene"); // scene
	scene_data_.name_ = main_node->first_attribute()->value();
	auto model_node = main_node->first_node(); // model
	while(model_node)
	{
		ModelData model_data;
		auto attr = model_node->first_attribute();
		model_data.file_ = attr->value();
		attr = attr->next_attribute();
		model_data.name_ = attr->value();

		auto node = model_node->first_node();
		while(node)
		{
			auto n = node->first_node();
			while(n)
			{
				if(std::string(node->name()) == std::string("material"))
				{
					attr = n->first_attribute();
					if(std::string(n->name()) == std::string("roughness"))
						model_data.material_.roughness_value_ = std::stof(attr->value());
					else if(std::string(n->name()) == std::string("metalness"))
						model_data.material_.metalness_value_ = std::stof(attr->value());
				}
				else if(std::string(node->name()) == std::string("transform"))
				{
					attr = n->first_attribute();
					if(std::string(n->name()) == std::string("position"))
					{
						model_data.transform_.pos_x_ = std::stof(attr->value());
						attr = attr->next_attribute();
						model_data.transform_.pos_y_ = std::stof(attr->value());
						attr = attr->next_attribute();
						model_data.transform_.pos_z_ = std::stof(attr->value());
					}
					else if(std::string(n->name()) == std::string("rotation"))
					{
						model_data.transform_.rot_x_ = std::stof(attr->value());
						attr = attr->next_attribute();
						model_data.transform_.rot_y_ = std::stof(attr->value());
						attr = attr->next_attribute();
						model_data.transform_.rot_z_ = std::stof(attr->value());
					}
					else
					{
						model_data.transform_.scale_x_ = std::stof(attr->value());
						attr = attr->next_attribute();
						model_data.transform_.scale_y_ = std::stof(attr->value());
						attr = attr->next_attribute();
						model_data.transform_.scale_z_ = std::stof(attr->value());
					}
				}
				else
				{
				}
				n = n->next_sibling();
			}
			node = node->next_sibling();
		}
		scene_data_.model_.push_back(model_data);
		model_node = model_node->next_sibling();
	}
	return true;
}