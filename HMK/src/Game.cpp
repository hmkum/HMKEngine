#include "Game.h"
#include <imgui/imgui.h>
#include "KeyManager.h"
#include "Utility.h"

Game::Game()
{
	mouse_right_pressed_ = false;
	mouse_left_pressed_  = false;
	light_position_ = glm::vec3(0, 4, 4);
}

Game::~Game()
{
}

bool Game::initialize()
{
    camera_ = std::make_shared<hmk::Camera>();
    camera_->create_look_at(glm::vec3(0.0f, 2.0f, 10.0f));
	camera_->create_perspective_proj(120.0f, 0.1f, 100.0f);

    shadow_map_ = std::make_shared<hmk::ShadowMap>();
    shadow_map_->initialize(2048, 2048);

	post_process_system_ = std::make_shared<hmk::PostProcess>();
	post_process_system_->initialize();

	hmk::Shader vert, frag;
	vert.initialize(GL_VERTEX_SHADER, "default.vert");
	frag.initialize(GL_FRAGMENT_SHADER, "default.frag");
	shader_basic_.add_shader(vert).add_shader(frag).link_shaders();

	vert.initialize(GL_VERTEX_SHADER, "skybox.vert");
	frag.initialize(GL_FRAGMENT_SHADER, "skybox.frag");
	shader_skybox_.add_shader(vert).add_shader(frag).link_shaders();
	
	vert.initialize(GL_VERTEX_SHADER, "simple_depth.vert");
	frag.initialize(GL_FRAGMENT_SHADER, "simple_depth.frag");
	shader_simple_depth_.add_shader(vert).add_shader(frag).link_shaders();

	vert.initialize(GL_VERTEX_SHADER, "post_process.vert");
	frag.initialize(GL_FRAGMENT_SHADER, "post_process.frag");
	shader_post_process_.add_shader(vert).add_shader(frag).link_shaders();

	glm::mat4 lightView = glm::lookAt(light_position_, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	light_projection_ = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 50.0f);
	light_space_matrix_ = light_projection_ * lightView;

	selected_model_with_mouse_ = std::make_shared<hmk::Model>();
	selected_model_with_mouse_.reset();

	model_plane_ = std::make_shared<hmk::Model>();
	model_plane_->load("plane.obj");
	model_plane_->scale(glm::vec3(10.0f));
	model_plane_->translate(glm::vec3(0.0f, -0.1f, -1.5f));
	model_plane_->set_roughness(1.0f);

	model_sphere_ = std::make_shared<hmk::Model>();
	model_sphere_->load("sphere.obj");

	model_sphere2_ = std::make_shared<hmk::Model>();
	model_sphere2_->load("sphere.obj");
	model_sphere2_->translate(glm::vec3(-3.0f, 0.0f, -2.0f));

	model_axe_ = std::make_shared<hmk::Model>();
	model_axe_->load("axe.obj");
	model_axe_->translate(glm::vec3(-30.0f, 0.0f, 0.0f));
	model_axe_->scale(glm::vec3(0.1f));
	model_axe_->rotate(90.0f, glm::vec3(1, 0, 0));
	model_axe_->rotate(180.0f, glm::vec3(0, 0, 1));

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

	static float r = 0.0f, m = 0.0f;
	if (selected_model_with_mouse_.get() != nullptr)
	{
		r = selected_model_with_mouse_->get_roughness();
		m = selected_model_with_mouse_->get_metallic();
	}
	ImGui::Begin("Material");
	ImGui::SliderFloat("Roughness", &r, 0.0f, 1.0f);
	ImGui::SliderFloat("Metallic", &m, 0.0f, 1.0f);
	ImGui::Separator();
	ImGui::DragFloat3("Light Position", (float*)&light_position_.x, 0.1f);
	ImGui::Separator();
	ImGui::SliderFloat("Tonemap Exposure", &gui_tonemap_exposure_, 0.0f, 16.0f);
	ImGui::Checkbox("Bloom", &gui_is_bloom_active_);
	ImGui::SliderFloat("Bloom Intensity", &gui_bloom_intensity_, 0.0f, 5.0f);
	ImGui::End();
	if (selected_model_with_mouse_.get() != nullptr)
	{
		selected_model_with_mouse_->set_roughness(r);
		selected_model_with_mouse_->set_metallic(m);
	}
	
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
