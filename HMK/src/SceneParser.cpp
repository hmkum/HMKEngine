#include "SceneParser.h"

using namespace hmk;

SceneData SceneParser::scene_data_;

std::string SceneParser::parse(const std::string& name)
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(name.c_str());
	if(!result)
	{
		return result.description();
	}
	pugi::xml_node scene_node = doc.first_child();
	scene_data_.name_ = scene_node.name();

	if(parse_camera(scene_node) == false)
	{
		return "Could not parse camera node!";
	}

	if(parse_atmosphere(scene_node) == false)
	{
		return "Could not parse atmosphere node!";
	}

	if(parse_models(scene_node) == false)
	{
		return "Could not parse models node!";
	}

	return "";
}

bool SceneParser::parse_camera(const pugi::xml_node& scene_node)
{
	// TODO_HMK: Check nodes for nullptr.
	CameraData camera_data;
	const pugi::xml_node camera_node = scene_node.child("camera");
	camera_data.name_ = camera_node.attribute("name").as_string();
	camera_data.projection_ = camera_node.attribute("projection").as_string();
	const pugi::xml_node prop_node = camera_node.child("properties");
	const pugi::xml_node pos_node = prop_node.child("position");
	camera_data.position_.x = pos_node.attribute("x").as_float();
	camera_data.position_.y = pos_node.attribute("y").as_float();
	camera_data.position_.z = pos_node.attribute("z").as_float();
	const pugi::xml_node target_node = prop_node.child("target");
	camera_data.target_.x = target_node.attribute("x").as_float();
	camera_data.target_.y = target_node.attribute("y").as_float();
	camera_data.target_.z = target_node.attribute("z").as_float();
	const pugi::xml_node right_node = prop_node.child("right");
	camera_data.right_.x = right_node.attribute("x").as_float();
	camera_data.right_.y = right_node.attribute("y").as_float();
	camera_data.right_.z = right_node.attribute("z").as_float();
	const pugi::xml_node up_node = prop_node.child("up");
	camera_data.up_.x = up_node.attribute("x").as_float();
	camera_data.up_.y = up_node.attribute("y").as_float();
	camera_data.up_.z = up_node.attribute("z").as_float();
	const pugi::xml_node persp_node = camera_node.child("perspective");
	camera_data.fov_ = persp_node.attribute("fov").as_float();
	const pugi::xml_node ortho_node = camera_node.child("orthographic");
	camera_data.ortho_params_.x = ortho_node.attribute("left").as_float();
	camera_data.ortho_params_.y = ortho_node.attribute("right").as_float();
	camera_data.ortho_params_.z = ortho_node.attribute("top").as_float();
	camera_data.ortho_params_.w = ortho_node.attribute("bottom").as_float();
	if(camera_data.projection_ == "perspective")
	{
		camera_data.near_z_ = ortho_node.attribute("nearZ").as_float();
		camera_data.far_z_ = ortho_node.attribute("farZ").as_float();
	}
	else if(camera_data.projection_ == "orthographic")
	{
		camera_data.near_z_ = persp_node.attribute("nearZ").as_float();
		camera_data.far_z_ = persp_node.attribute("farZ").as_float();
	}
	else
	{
		return "Camera projection type is wrong!";
	}

	scene_data_.camera_ = camera_data;
	return true;
}

bool SceneParser::parse_models(const pugi::xml_node& scene_node)
{
	// TODO_HMK: Check nodes for nullptr.
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

		scene_data_.models_.push_back(model_data);
	}
	return true;
}

bool SceneParser::parse_atmosphere(const pugi::xml_node& scene_node)
{
	// TODO_HMK: Check nodes for nullptr.
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
	return true;
}
