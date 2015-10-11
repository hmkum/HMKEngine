#include "Game.h"
#include <imgui/imgui.h>
#include <filesystem> // C++17
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

	model_plane_ = std::make_shared<hmk::Model>();
	model_plane_->load("plane.obj");
	model_plane_->set_scale(glm::vec3(10.0f));
	model_plane_->set_roughness(1.0f);

	model_sphere_ = std::make_shared<hmk::Model>();
	model_sphere_->load("sphere.obj");

	model_sphere2_ = std::make_shared<hmk::Model>();
	model_sphere2_->load("sphere.obj");
	model_sphere2_->set_position(glm::vec3(-3.0f, 0.0f, 0.0f));

	model_axe_ = std::make_shared<hmk::Model>();
	model_axe_->load("axe.obj");
	model_axe_->set_position(glm::vec3(2.0f, 0.0f, 0.0f));
	model_axe_->set_scale(glm::vec3(0.1f));
	model_axe_->set_rotation(90.0f, 0.0f, 0.0f);

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
		ImGui::Separator();
		ImGui::SliderFloat("Tonemap Exposure", &gui_tonemap_exposure_, 0.0f, 16.0f);
		ImGui::Checkbox("Bloom", &gui_is_bloom_active_);
		ImGui::SliderFloat("Bloom Intensity", &gui_bloom_intensity_, 0.0f, 5.0f);
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

	shader_simple_depth_.set_uniform("uModel", model_plane_->get_model_matrix());
	model_plane_->render();

	shader_simple_depth_.set_uniform("uModel", model_sphere_->get_model_matrix());
	model_sphere_->render();

	shader_simple_depth_.set_uniform("uModel", model_sphere2_->get_model_matrix());
	model_sphere2_->render();

	shader_simple_depth_.set_uniform("uModel", model_axe_->get_model_matrix());
	model_axe_->render();

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

		shader_basic_.set_uniform("uModel", model_plane_->get_model_matrix());
		model_plane_->render(shader_basic_);

		shader_basic_.set_uniform("uModel", model_sphere_->get_model_matrix());
		model_sphere_->render(shader_basic_);

		shader_basic_.set_uniform("uModel", model_sphere2_->get_model_matrix());
		model_sphere2_->render(shader_basic_);

		shader_basic_.set_uniform("uModel", model_axe_->get_model_matrix());
		model_axe_->render(shader_basic_);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	post_process_system_->end();
	const glm::mat4 viewProjMatrix = camera_->get_proj_matrix() * camera_->get_view_matrix();

	if(gui_is_bloom_active_) post_process_system_->do_bloom(gui_bloom_intensity_);
	post_process_system_->do_motion_blur(viewProjMatrix);
	
	post_process_system_->do_hdr(gui_tonemap_exposure_, gui_is_bloom_active_);
	post_process_system_->render(shader_post_process_);
}

void Game::process_selection(int x, int y)
{
	glm::vec3 rayNear = glm::unProject(glm::vec3(x, 600 - y, 0.0f), camera_->get_view_matrix(), camera_->get_proj_matrix(), glm::vec4(0, 0, 800, 600));
	glm::vec3 rayFar = glm::unProject(glm::vec3(x, 600 - y, 1.0f), camera_->get_view_matrix(), camera_->get_proj_matrix(), glm::vec4(0, 0, 800, 600));
	
	hmk::Ray ray(camera_->get_position(), normalize(rayFar - rayNear));

	// TODO: Get model's bounding box from ModelManager
	std::vector<hmk::BoundingBox> boxes;
	boxes.push_back(model_sphere_->get_bounding_box());
	boxes.push_back(model_sphere2_->get_bounding_box());
	boxes.push_back(model_axe_->get_bounding_box());

	for (unsigned int i = 0; i < boxes.size(); ++i)
	{
		hmk::BoundingBox box = boxes[i];
		if(ray.intersect_aabb(box))
		{
			// TODO: Implement ModelManager and select model with index
			switch(i)
			{
				case 0: selected_model_with_mouse_ = model_sphere_; break;
				case 1: selected_model_with_mouse_ = model_sphere2_; break;
				case 2: selected_model_with_mouse_ = model_axe_; break;
				default:
					break;
			}
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
