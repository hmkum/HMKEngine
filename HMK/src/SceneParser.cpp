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
	if(scene_node.empty())
	{
		return "Scene is empty!";
	}
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
	pugi::xml_node camera_node = scene_node.child("cameras");
	camera_node = camera_node.first_child();
	while(camera_node)
	{
		CameraData camera_data;
		camera_data.name_ = camera_node.attribute("name").as_string();
		camera_data.projection_ = camera_node.attribute("projection").as_string();
		const pugi::xml_node prop_node = camera_node.child("properties");
		if(prop_node.empty())
			return false;
		const pugi::xml_node pos_node = prop_node.child("position");
		if(pos_node.empty())
			return false;
		camera_data.position_.x = pos_node.attribute("x").as_float();
		camera_data.position_.y = pos_node.attribute("y").as_float();
		camera_data.position_.z = pos_node.attribute("z").as_float();
		const pugi::xml_node target_node = prop_node.child("target");
		if(target_node.empty())
			return false;
		camera_data.target_.x = target_node.attribute("x").as_float();
		camera_data.target_.y = target_node.attribute("y").as_float();
		camera_data.target_.z = target_node.attribute("z").as_float();
		const pugi::xml_node up_node = prop_node.child("up");
		if(up_node.empty())
			return false;
		camera_data.up_.x = up_node.attribute("x").as_float();
		camera_data.up_.y = up_node.attribute("y").as_float();
		camera_data.up_.z = up_node.attribute("z").as_float();
		const pugi::xml_node rotation_node = prop_node.child("rotation");
		if(rotation_node.empty())
			return false;
		camera_data.yaw_ = rotation_node.attribute("yaw").as_float();
		camera_data.pitch_ = rotation_node.attribute("pitch").as_float();
		bool proj_persp_result = true;
		const pugi::xml_node persp_node = camera_node.child("perspective");
		if(persp_node.empty())
			proj_persp_result = false;
		else
			camera_data.fov_ = persp_node.attribute("fov").as_float();
		bool proj_ortho_result = true;
		const pugi::xml_node ortho_node = camera_node.child("orthographic");
		if(ortho_node.empty())
			proj_ortho_result = false;
		else
		{
			camera_data.ortho_params_.x = ortho_node.attribute("left").as_float();
			camera_data.ortho_params_.y = ortho_node.attribute("right").as_float();
			camera_data.ortho_params_.z = ortho_node.attribute("top").as_float();
			camera_data.ortho_params_.w = ortho_node.attribute("bottom").as_float();
		}

		if(proj_persp_result)
		{
			camera_data.near_z_ = persp_node.attribute("nearZ").as_float();
			camera_data.far_z_ = persp_node.attribute("farZ").as_float();
		}
		else if(proj_ortho_result)
		{
			camera_data.near_z_ = ortho_node.attribute("nearZ").as_float();
			camera_data.far_z_ = ortho_node.attribute("farZ").as_float();
		}
		else
		{
			return false;
		}
		camera_node = camera_node.next_sibling();
		scene_data_.cameras_.push_back(camera_data);
	}
	return true;
}

bool SceneParser::parse_models(const pugi::xml_node& scene_node)
{
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
