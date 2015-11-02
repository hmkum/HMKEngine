#include "Game.h"
#include <imgui/imgui.h>
#include <pugixml/pugixml.hpp>
#include <filesystem> // C++17
#include <set>
#include "KeyManager.h"
#include "SceneParser.h"
#include "ShaderManager.h"
#include "Utility.h"

Game::Game()
{
}

Game::~Game()
{
}

bool Game::initialize()
{
	//compile_and_link_all_shaders();

	std::unique_ptr<hmk::Scene> scene = std::make_unique<hmk::Scene>();
	scene->initialize();

	std::string scene_parse_result = hmk::SceneParser::parse("data/scene.xml");
	if(scene_parse_result != "")
	{
		HMK_LOG_ERROR("Could not parse scene.xml. Error: ", scene_parse_result)
		return false;
	}
	hmk::SceneData scene_data = hmk::SceneParser::get_data();
	for(const auto camera_data : scene_data.cameras_)
	{
		auto camera = std::make_shared<hmk::Camera>();
		camera->create_look_at(camera_data.position_);
		if(camera_data.projection_ == "perspective")
			camera->create_perspective_proj(camera_data.fov_, camera_data.near_z_, camera_data.far_z_);
		else
			camera->create_orthographic_proj(camera_data.ortho_params_, camera_data.near_z_, camera_data.far_z_);

		camera->rotate(camera_data.yaw_ + 90.f * 4, camera_data.pitch_);
		scene->add_camera(camera);
	}

	for(const auto& model : scene_data.models_)
	{
		HMK_PRINT("Loading model: " + model.file_);
		hmk::ModelSPtr temp_model = std::make_shared<hmk::Model>();
		if(!temp_model->load(model.file_))
		{
			HMK_LOG_WARNING("Could not load " + model.file_);
			HMK_PRINT("Could not load " + model.file_);
			continue;
		}
		temp_model->set_name(model.name_);
		temp_model->set_roughness(model.material_.roughness_value_);
		temp_model->set_metallic(model.material_.metalness_value_);
		temp_model->set_position(glm::vec3(model.transform_.pos_x_, model.transform_.pos_y_, model.transform_.pos_z_));
		temp_model->set_rotation(model.transform_.rot_x_, model.transform_.rot_y_, model.transform_.rot_z_);
		temp_model->set_scale(glm::vec3(model.transform_.scale_x_, model.transform_.scale_y_, model.transform_.scale_z_));
		scene->add_entity(temp_model);
		HMK_PRINT("Done: " + model.file_);
	}

	std::unique_ptr<hmk::Skybox> skybox_ = std::make_unique<hmk::Skybox>();
	skybox_->load(scene_data.atmosphere_.skybox_folder_);
	scene->set_skybox(std::move(skybox_));

	scenes_.emplace_back(std::move(scene));
	return true;
}

void Game::update(float dt)
{
	scenes_[0]->update(dt);
}

void Game::render()
{	
	scenes_[0]->render();
}

void Game::key_input(int key, int scancode, int action, int mods)
{
	hmk::KeyManager::set_key(key, (action == HMK_RELEASE) ? false : true);
	if(key == HMK_KEY_ESCAPE)
	{
		exit(1);
	}

	scenes_[0]->key_input(key, scancode, action, mods);
}

void Game::cursor_pos_input(double xPos, double yPos)
{
	if(cursor_state_.first_mouse_)
	{
		cursor_state_.last_position_ = glm::vec2(xPos, yPos);
		cursor_state_.first_mouse_ = false;
	}

	double xOffset = xPos - cursor_state_.last_position_.x;
	double yOffset = cursor_state_.last_position_.y - yPos;
	cursor_state_.last_position_ = glm::vec2(xPos, yPos);
	
	scenes_[0]->cursor_pos_input(xPos, yPos, (float)xOffset, (float)yOffset);
}

void Game::mouse_button_input(int button, int action, int mods)
{
	scenes_[0]->mouse_button_input(button, action, mods);
}

void Game::drop_files_callback(int number_of_files, const char** filenames)
{
	std::vector<std::string> model_names;
	std::string filename;
	std::string file_proper_name;
	for(int i = 0; i < number_of_files; ++i)
	{
		std::string file_path(filenames[i]);
		auto last_slash = file_path.rfind("\\");
		filename = file_path;
		file_path.erase(file_path.begin() + last_slash + 1, file_path.end());
		filename.erase(filename.begin(), filename.begin() + last_slash + 1);
		std::string file_extension = filename.substr(filename.rfind("."));
		file_proper_name = filename;
		file_proper_name.erase(filename.find(file_extension));

		HMK_PRINT("Copying " + filename);
		if(file_extension.compare(".obj") == 0 || file_extension.compare(".mtl") == 0)
		{
			if(file_extension.compare(".obj") == 0)
				model_names.push_back(filename);
			hmk::copy_file(file_path + filename, WORKIND_DIR + "data\\models\\" + filename);
		}
		if(file_extension.compare(".jpg") == 0 || file_extension.compare(".JPG") == 0 || file_extension.compare(".tga") == 0
		   || file_extension.compare(".png") == 0)
			hmk::copy_file(file_path + filename, WORKIND_DIR + "data\\textures\\" + filename, true);

		HMK_PRINT("Done " + filename);
	}

	/*for(unsigned int i = 0; i < model_names.size(); ++i)
	{
		filename = model_names[i];
		HMK_PRINT("Loading " + filename);
		hmk::ModelUPtr temp_model = std::make_unique<hmk::Model>();
		temp_model->load(filename);
		temp_model->set_name(file_proper_name);
		temp_model->set_position(glm::vec3(0.0f));
		scene_models.emplace_back(std::move(temp_model));
		HMK_PRINT("Done " + filename);
	}*/
}

bool Game::compile_and_link_all_shaders()
{
	namespace fs = std::tr2::sys;
	std::set<std::string> shader_names_set, post_process_shader_names_set;
	fs::path shaders_path{SHADER_PATH};
	for(auto it = fs::directory_iterator(shaders_path); it != fs::directory_iterator(); ++it)
	{
		const auto& file = it->path();
		std::string filename = file.basename();
		auto is_pp_shader = filename.find("pp_");
		if(file.extension().compare(".vert") == 0 || file.extension().compare(".frag") == 0)
		{
			if(is_pp_shader == std::string::npos)
			{
				shader_names_set.insert(filename);
			}
			else
			{
				if(filename.find("pp_default") != std::string::npos)
					continue;
				post_process_shader_names_set.insert(filename);
			}
		}
	}
	
	std::vector<std::string> shader_names(shader_names_set.size());
	std::vector<std::string> post_process_shader_names(post_process_shader_names_set.size());
	shader_names.assign(std::begin(shader_names_set), std::cend(shader_names_set));
	post_process_shader_names.assign(std::begin(post_process_shader_names_set), std::cend(post_process_shader_names_set));
	
	bool result = true;
	for(unsigned int i = 0; i < shader_names.size(); ++i)
	{
		std::string shader_name = shader_names[i];
		hmk::ShaderManager::get_instance().add_shader(shader_name);
	}

	for(unsigned int i = 0; i < post_process_shader_names.size(); ++i)
	{
		std::string shader_name = post_process_shader_names[i];
		hmk::ShaderManager::get_instance().add_shader("pp_default", shader_name);
	}

	hmk::ShaderManager::get_instance().compile_and_link_shaders();

	return true;
}
