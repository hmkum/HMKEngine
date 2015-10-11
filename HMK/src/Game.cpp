#include "Game.h"
#include <imgui/imgui.h>
#include <rapidxml/rapidxml.hpp>
#include <filesystem> // C++17
#include <fstream>
#include <streambuf>
#include <set>
#include "KeyManager.h"
#include "ShaderManager.h"
#include "Utility.h"

Game::Game()
{
	mouse_right_pressed_ = false;
	mouse_left_pressed_  = false;
	light_position_		 = glm::vec3(0, 4, 4);
	selected_model_position_ = glm::vec3(0);
	selected_model_rotation_ = glm::vec3(0);
	selected_model_scale_	 = glm::vec3(0);
}

Game::~Game()
{
}

bool Game::initialize()
{
    camera_ = std::make_shared<hmk::Camera>();
    camera_->create_look_at(glm::vec3(0.0f, 2.0f, 10.0f));
	camera_->create_perspective_proj(120.0f, 0.1f, 100.0f);

	compile_and_link_all_shaders();

    shadow_map_ = std::make_shared<hmk::ShadowMap>();
    shadow_map_->initialize(2048, 2048);

	post_process_system_ = std::make_shared<hmk::PostProcess>();
	post_process_system_->initialize();

	shader_basic_.add_shader("default.vert", "default.frag");
	shader_basic_.link_shaders();

	shader_skybox_.add_shader("skybox.vert", "skybox.frag");
	shader_skybox_.link_shaders();
	
	shader_simple_depth_.add_shader("simple_depth.vert", "simple_depth.frag");
	shader_simple_depth_.link_shaders();

	shader_post_process_.add_shader("post_process.vert", "post_process.frag");
	shader_post_process_.link_shaders();

	glm::mat4 lightView = glm::lookAt(light_position_, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	light_projection_ = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 50.0f);
	light_space_matrix_ = light_projection_ * lightView;

	selected_model_with_mouse_ = std::make_shared<hmk::Model>();
	selected_model_with_mouse_.reset();

	std::ifstream scene_file("data/scene.xml");
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
	auto model_node = main_node->first_node(); // model
	while(model_node)
	{
		// TODO_HMK: Improve with loops
		auto node = model_node;
		auto attr = node->first_attribute();
		std::string model_file = attr->value();
		attr = attr->next_attribute();
		std::string model_name = attr->value();

		node = node->first_node(); // boundingbox
		attr = node->first_attribute();
		std::string bounding_box_draw_str = attr->value();
		bool bounding_box_draw = std::stoi(bounding_box_draw_str) == 0 ? false : true;

		node = node->next_sibling(); // material
		auto mat_node = node->first_node(); // roughness
		attr = mat_node->first_attribute();
		std::string roughness_value_str = attr->value();
		float roughness_value = std::stof(roughness_value_str);
		mat_node = mat_node->next_sibling(); // metalness
		attr = mat_node->first_attribute();
		std::string metalness_value_str = attr->value();
		float metalness_value = std::stof(metalness_value_str);

		node = node->next_sibling(); // transform
		auto trans_node = node->first_node(); // position
		attr = trans_node->first_attribute();
		std::string pos_x_str = attr->value();
		float pos_x = std::stof(pos_x_str);
		attr = attr->next_attribute();
		std::string pos_y_str = attr->value();
		float pos_y = std::stof(pos_y_str);
		attr = attr->next_attribute();
		std::string pos_z_str = attr->value();
		float pos_z = std::stof(pos_z_str);

		trans_node = trans_node->next_sibling(); // rotation
		attr = trans_node->first_attribute();
		std::string rot_x_str = attr->value();
		float rot_x = std::stof(rot_x_str);
		attr = attr->next_attribute();
		std::string rot_y_str = attr->value();
		float rot_y = std::stof(rot_y_str);
		attr = attr->next_attribute();
		std::string rot_z_str = attr->value();
		float rot_z = std::stof(rot_z_str);

		trans_node = trans_node->next_sibling(); // scale
		attr = trans_node->first_attribute();
		std::string scale_x_str = attr->value();
		float scale_x = std::stof(scale_x_str);
		attr = attr->next_attribute();
		std::string scale_y_str = attr->value();
		float scale_y = std::stof(scale_y_str);
		attr = attr->next_attribute();
		std::string scale_z_str = attr->value();
		float scale_z = std::stof(scale_z_str);

		hmk::ModelPtr temp_model = std::make_shared<hmk::Model>();
		temp_model->load(model_file);
		temp_model->set_name(model_name);
		temp_model->draw_bounding_box(bounding_box_draw);
		temp_model->set_roughness(roughness_value);
		temp_model->set_metallic(metalness_value);
		temp_model->set_position(glm::vec3(pos_x, pos_y, pos_z));
		temp_model->set_rotation(rot_x, rot_y, rot_z);
		temp_model->set_scale(glm::vec3(scale_x, scale_y, scale_z));
		scene_models.push_back(temp_model);

		model_node = model_node->next_sibling();
	}

	skybox_ = std::make_shared<hmk::Skybox>();
	skybox_->load("Bridge/");

	return true;
}

void Game::update(float dt)
{
	if (hmk::KeyManager::get_key(HMK_KEY_W))
		camera_->move_forward(dt);
	if (hmk::KeyManager::get_key(HMK_KEY_S))
		camera_->move_backward(dt);
	if (hmk::KeyManager::get_key(HMK_KEY_A))
		camera_->move_left(dt);
	if (hmk::KeyManager::get_key(HMK_KEY_D))
		camera_->move_right(dt);

	ImGui::Begin("Properties");
	if(ImGui::CollapsingHeader("World Properties"))
	{
		ImGui::DragFloat3("Light Position", (float*)&light_position_.x, 0.1f);
	}
	if(ImGui::CollapsingHeader("Post Process Effects"))
	{
		ImGui::Checkbox("Bloom", &gui_is_bloom_active_);
		if(gui_is_bloom_active_) ImGui::SliderFloat("Bloom Intensity", &gui_bloom_intensity_, 0.0f, 5.0f);
		ImGui::SliderFloat("Tonemap Exposure", &gui_tonemap_exposure_, 0.0f, 16.0f);
		ImGui::Checkbox("Motion BLur", &gui_is_motionblur_active_);
		ImGui::Checkbox("Monochrome", &gui_is_monochrome_active_);
		ImGui::Checkbox("Negative", &gui_is_negative_active_);
	}
	static float r = 0.0f, m = 0.0f;
	if(selected_model_with_mouse_.get() != nullptr)
	{
		r = selected_model_with_mouse_->get_roughness();
		m = selected_model_with_mouse_->get_metallic();
		selected_model_position_ = selected_model_with_mouse_->get_position();
		selected_model_rotation_ = selected_model_with_mouse_->get_rotation();
		selected_model_scale_	 = selected_model_with_mouse_->get_scale();

		if(ImGui::CollapsingHeader("Entity Properties"))
		{
			ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), selected_model_with_mouse_->get_name().c_str());
			ImGui::SliderFloat("Roughness", &r, 0.0f, 1.0f);
			ImGui::SliderFloat("Metallic", &m, 0.0f, 1.0f);
			selected_model_with_mouse_->set_roughness(r);
			selected_model_with_mouse_->set_metallic(m);

			ImGui::Separator();

			ImGui::DragFloat3("Position", (float*)&selected_model_position_.x, 0.01f);
			selected_model_with_mouse_->set_position(selected_model_position_);

			ImGui::DragFloat3("Rotation", (float*)&selected_model_rotation_.x, 0.1f);
			selected_model_with_mouse_->set_rotation(selected_model_rotation_);

			ImGui::DragFloat3("Scale", (float*)&selected_model_scale_.x, 0.01f);
			selected_model_with_mouse_->set_scale(selected_model_scale_);
		}
	}
	ImGui::End();
	
	glm::mat4 lightView = glm::lookAt(light_position_, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	light_space_matrix_ = light_projection_ * lightView;
}

void Game::render()
{	
	shadow_map_->bind();

	shader_simple_depth_.use();
	shader_simple_depth_.set_uniform("uLightSpaceMatrix", light_space_matrix_);

	for(const auto& model : scene_models)
	{
		shader_simple_depth_.set_uniform("uModel", model->get_model_matrix());
		model->render();
	}

	shadow_map_->unbind();
	
	glViewport(0, 0, 800, 600);

	post_process_system_->begin();
		shader_skybox_.use();
		glm::mat4 skyboxModelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(40.0f));
		shader_skybox_.set_uniform("uModel", skyboxModelMatrix);
		shader_skybox_.set_uniform("uViewMatrix", camera_->get_view_matrix());
		shader_skybox_.set_uniform("uProjMatrix", camera_->get_proj_matrix());
		skybox_->render();

		shader_basic_.use();

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_->get_texture_id());

		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, shadow_map_->get_depth_map());
		
		shader_basic_.set_uniform("uViewMatrix", camera_->get_view_matrix());
		shader_basic_.set_uniform("uProjMatrix", camera_->get_proj_matrix());
		shader_basic_.set_uniform("uCameraPosition", camera_->get_position());
		shader_basic_.set_uniform("uLightPosition", light_position_);
		shader_basic_.set_uniform("uLightSpaceMatrix", light_space_matrix_);

		for(const auto& model : scene_models)
		{
			shader_basic_.set_uniform("uModel", model->get_model_matrix());
			model->render(shader_basic_);
		}

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	post_process_system_->end();
	const glm::mat4 viewProjMatrix = camera_->get_proj_matrix() * camera_->get_view_matrix();

	if(gui_is_bloom_active_) post_process_system_->do_bloom(gui_bloom_intensity_);
	if(gui_is_motionblur_active_) post_process_system_->do_motion_blur(viewProjMatrix);
	if(gui_is_monochrome_active_) post_process_system_->do_monochrome();
	if(gui_is_negative_active_) post_process_system_->do_negative();
	post_process_system_->do_hdr(gui_tonemap_exposure_, gui_is_bloom_active_);
	post_process_system_->render(shader_post_process_);
}

void Game::process_selection(int x, int y)
{
	glm::vec3 rayNear = glm::unProject(glm::vec3(x, 600 - y, 0.0f), camera_->get_view_matrix(), camera_->get_proj_matrix(), glm::vec4(0, 0, 800, 600));
	glm::vec3 rayFar = glm::unProject(glm::vec3(x, 600 - y, 1.0f), camera_->get_view_matrix(), camera_->get_proj_matrix(), glm::vec4(0, 0, 800, 600));
	
	hmk::Ray ray(camera_->get_position(), normalize(rayFar - rayNear));

	std::vector<hmk::BoundingBox> boxes;
	for(const auto& model : scene_models)
	{
		boxes.push_back(model->get_bounding_box());
	}

	for (unsigned int i = 0; i < boxes.size(); ++i)
	{
		hmk::BoundingBox box = boxes[i];
		if(ray.intersect_aabb(box))
		{
			selected_model_with_mouse_ = scene_models[i];
		}
	}
}

void Game::key_input(int key, int scancode, int action, int mods)
{
	hmk::KeyManager::set_key(key, (action == HMK_RELEASE) ? false : true);
	if(key == HMK_KEY_ESCAPE)
	{
		exit(1);
	}
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

	if(mouse_right_pressed_)
		camera_->rotate((float)xOffset, (float)yOffset);

	if(mouse_left_pressed_)
	{
		int x = (int)xPos;
		int y = (int)yPos;
		process_selection(x, y);
	}
}

void Game::mouse_button_input(int button, int action, int mods)
{
	if (button == HMK_MOUSE_BUTTON_RIGHT && action == HMK_PRESS)
		mouse_right_pressed_ = true;
	else
		mouse_right_pressed_ = false;

	if (button == HMK_MOUSE_BUTTON_LEFT && action == HMK_PRESS)
		mouse_left_pressed_ = true;
	else
		mouse_left_pressed_ = false;
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
