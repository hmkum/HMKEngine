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
	mouse_right_pressed_ = false;
	mouse_left_pressed_  = false;
	light_position_		 = glm::vec3(0, 4, 4);
	selected_model_position_ = glm::vec3(0);
	selected_model_rotation_ = glm::vec3(0);
	selected_model_scale_	 = glm::vec3(0);
	selected_model_index	 = -1;
}

Game::~Game()
{
}

bool Game::initialize()
{
	//compile_and_link_all_shaders();

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
		cameras_.push_back(camera);
	}
	for(const auto& model : scene_data.models_)
	{
		HMK_PRINT("Loading model: " + model.file_);
		hmk::ModelUPtr temp_model = std::make_unique<hmk::Model>();
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
		scene_models.emplace_back(std::move(temp_model));
		HMK_PRINT("Done: " + model.file_);
	}

	skybox_ = std::make_shared<hmk::Skybox>();
	skybox_->load(scene_data.atmosphere_.skybox_folder_);

	return true;
}

void Game::update(float dt)
{
	if (hmk::KeyManager::get_key(HMK_KEY_W))
		cameras_[0]->move_forward(dt);
	if (hmk::KeyManager::get_key(HMK_KEY_S))
		cameras_[0]->move_backward(dt);
	if (hmk::KeyManager::get_key(HMK_KEY_A))
		cameras_[0]->move_left(dt);
	if (hmk::KeyManager::get_key(HMK_KEY_D))
		cameras_[0]->move_right(dt);

	ImGui::Begin("Properties");
	if(ImGui::CollapsingHeader("World Properties"))
	{
		ImGui::DragFloat3("Light Position", (float*)&light_position_.x, 0.1f);
		ImGui::Checkbox("Shadows", &gui_is_shadow_active);
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
	if(selected_model_index != -1)
	{
		if(ImGui::CollapsingHeader("Entity Properties"))
		{
			r = scene_models[selected_model_index]->get_roughness();
			m = scene_models[selected_model_index]->get_metallic();
			selected_model_position_ = scene_models[selected_model_index]->get_position();
			selected_model_rotation_ = scene_models[selected_model_index]->get_rotation();
			selected_model_scale_ = scene_models[selected_model_index]->get_scale();

			ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), scene_models[selected_model_index]->get_name().c_str());
			if(ImGui::SliderFloat("Roughness", &r, 0.0f, 1.0f))
				scene_models[selected_model_index]->set_roughness(r);
			if(ImGui::SliderFloat("Metallic", &m, 0.0f, 1.0f))
				scene_models[selected_model_index]->set_metallic(m);

			ImGui::Separator();

			if(ImGui::DragFloat3("Position", (float*)&selected_model_position_.x, 0.1f))
				scene_models[selected_model_index]->set_position(selected_model_position_);

			if(ImGui::DragFloat3("Rotation", (float*)&selected_model_rotation_.x, 0.1f))
				scene_models[selected_model_index]->set_rotation(selected_model_rotation_);

			if(ImGui::DragFloat3("Scale", (float*)&selected_model_scale_.x, 0.1f))
				scene_models[selected_model_index]->set_scale(selected_model_scale_);
		}
	}
	ImGui::End();
	
	is_mouse_on_gui =  ImGui::IsMouseHoveringAnyWindow();

	glm::mat4 lightView = glm::lookAt(light_position_, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	light_space_matrix_ = light_projection_ * lightView;
}

void Game::render()
{	
	if(gui_is_shadow_active)
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
	}
	post_process_system_->begin();
		shader_skybox_.use();
		glm::mat4 skyboxModelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(40.0f));
		shader_skybox_.set_uniform("uModel", skyboxModelMatrix);
		shader_skybox_.set_uniform("uViewMatrix", cameras_[0]->get_view_matrix());
		shader_skybox_.set_uniform("uProjMatrix", cameras_[0]->get_proj_matrix());
		skybox_->render();

		shader_basic_.use();

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_->get_texture_id());

		shader_basic_.set_uniform("uIsShadowMapActive", 0);
		if(gui_is_shadow_active)
		{
			shader_basic_.set_uniform("uIsShadowMapActive", 1);
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, shadow_map_->get_depth_map());
		}
		
		shader_basic_.set_uniform("uViewMatrix", cameras_[0]->get_view_matrix());
		shader_basic_.set_uniform("uProjMatrix", cameras_[0]->get_proj_matrix());
		shader_basic_.set_uniform("uCameraPosition", cameras_[0]->get_position());
		shader_basic_.set_uniform("uLightPosition", light_position_);
		shader_basic_.set_uniform("uLightSpaceMatrix", light_space_matrix_);

		for(const auto& model : scene_models)
		{
			shader_basic_.set_uniform("uModel", model->get_model_matrix());
			model->render(shader_basic_);
		}

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	post_process_system_->end();
	const glm::mat4 viewProjMatrix = cameras_[0]->get_proj_matrix() * cameras_[0]->get_view_matrix();

	if(gui_is_bloom_active_) post_process_system_->do_bloom(gui_bloom_intensity_);
	if(gui_is_motionblur_active_) post_process_system_->do_motion_blur(viewProjMatrix);
	if(gui_is_monochrome_active_) post_process_system_->do_monochrome();
	if(gui_is_negative_active_) post_process_system_->do_negative();
	post_process_system_->do_hdr(gui_tonemap_exposure_, gui_is_bloom_active_);
	post_process_system_->render(shader_post_process_);
}

void Game::process_selection(int x, int y)
{
	glm::vec3 rayNear = glm::unProject(glm::vec3(x, 600 - y, 0.0f), cameras_[0]->get_view_matrix(), cameras_[0]->get_proj_matrix(), glm::vec4(0, 0, 800, 600));
	glm::vec3 rayFar = glm::unProject(glm::vec3(x, 600 - y, 1.0f), cameras_[0]->get_view_matrix(), cameras_[0]->get_proj_matrix(), glm::vec4(0, 0, 800, 600));
	
	hmk::Ray ray(cameras_[0]->get_position(), normalize(rayFar - rayNear));
	int i = 0;
	for(const auto& model : scene_models)
	{
		hmk::BoundingBox box = model->get_bounding_box();
		if(ray.intersect_aabb(box))
		{
			selected_model_index = i;
		}
		++i;
	}
}

void Game::key_input(int key, int scancode, int action, int mods)
{
	hmk::KeyManager::set_key(key, (action == HMK_RELEASE) ? false : true);

	if(key == HMK_KEY_S && mods == HMK_MOD_CONTROL)
	{
		std::string xml_text = "<?xml version=\"1.0\" encoding=\"UTF - 8\"?>";
		xml_text += "<scene name = \"default\">";
		xml_text += "</scene>";
		pugi::xml_document doc;
		doc.load_string(xml_text.c_str());
		pugi::xml_node scene_node = doc.first_child();

		pugi::xml_node cameras_node = scene_node.append_child("cameras");
		for(const auto& camera_ : cameras_)
		{
			// Add cameras
			pugi::xml_node camera_node = cameras_node.append_child("camera");
			camera_node.append_attribute("name").set_value("Main Camera");
			camera_node.append_attribute("projection").set_value("perspective");
			pugi::xml_node camera_prop_node = camera_node.append_child("properties");
			pugi::xml_node camera_prop_pos_node = camera_prop_node.append_child("position");
			const glm::vec3 camera_pos = camera_->get_position();
			camera_prop_pos_node.append_attribute("x").set_value(camera_pos.x);
			camera_prop_pos_node.append_attribute("y").set_value(camera_pos.y);
			camera_prop_pos_node.append_attribute("z").set_value(camera_pos.z);
			pugi::xml_node camera_prop_target_node = camera_prop_node.append_child("target");
			const glm::vec3 camera_target = camera_->get_target();
			camera_prop_target_node.append_attribute("x").set_value(camera_target.x);
			camera_prop_target_node.append_attribute("y").set_value(camera_target.y);
			camera_prop_target_node.append_attribute("z").set_value(camera_target.z);
			pugi::xml_node camera_prop_up_node = camera_prop_node.append_child("up");
			const glm::vec3 camera_up = camera_->get_up_vector();
			camera_prop_up_node.append_attribute("x").set_value(camera_up.x);
			camera_prop_up_node.append_attribute("y").set_value(camera_up.y);
			camera_prop_up_node.append_attribute("z").set_value(camera_up.z);
			pugi::xml_node camera_rotation_node = camera_prop_node.append_child("rotation");
			camera_rotation_node.append_attribute("yaw").set_value(camera_->get_yaw());
			camera_rotation_node.append_attribute("pitch").set_value(camera_->get_pitch());
			if(camera_->get_projection() == hmk::CameraProjection::Perspective)
			{
				pugi::xml_node camera_persp_node = camera_node.append_child("perspective");
				camera_persp_node.append_attribute("fov").set_value(camera_->get_fov());
				camera_persp_node.append_attribute("nearZ").set_value(camera_->get_near_z());
				camera_persp_node.append_attribute("farZ").set_value(camera_->get_far_z());
			}
			else
			{
				pugi::xml_node camera_ortho_node = camera_node.append_child("orthographic");
				const glm::vec4 ortho_params = camera_->get_ortho_params();
				camera_ortho_node.append_attribute("left").set_value(ortho_params.x);
				camera_ortho_node.append_attribute("right").set_value(ortho_params.y);
				camera_ortho_node.append_attribute("top").set_value(ortho_params.z);
				camera_ortho_node.append_attribute("bottom").set_value(ortho_params.w);
				camera_ortho_node.append_attribute("nearZ").set_value(camera_->get_near_z());
				camera_ortho_node.append_attribute("farZ").set_value(camera_->get_far_z());
			}
		}
		// Atmosphere things
		pugi::xml_node atmosphere_node = scene_node.append_child("atmosphere");
		
		// Add skybox
		pugi::xml_node skybox_node = atmosphere_node.append_child("skybox");
		skybox_node.append_attribute("folder").set_value(skybox_->get_folder_name().c_str());

		// Add fog
		pugi::xml_node fog_node = atmosphere_node.append_child("fog");
		fog_node.append_attribute("method").set_value("linear");
		fog_node.append_attribute("density").set_value("0.2");
		fog_node.append_attribute("start").set_value("5");
		fog_node.append_attribute("end").set_value("100");

		// Add models
		for(const auto& model : scene_models)
		{
			pugi::xml_node model_node = scene_node.append_child("model");
			model_node.append_attribute("file").set_value(model->get_filename().c_str());
			model_node.append_attribute("name").set_value(model->get_name().c_str());
			pugi::xml_node material_node = model_node.append_child("material");
			material_node.append_child("roughness").append_attribute("value").set_value(model->get_roughness());
			material_node.append_child("metalness").append_attribute("value").set_value(model->get_metallic());
			pugi::xml_node transform_node = model_node.append_child("transform");
			pugi::xml_node pos_node = transform_node.append_child("position");
			glm::vec3 pos = model->get_position();
			pos_node.append_attribute("x").set_value(pos.x);
			pos_node.append_attribute("y").set_value(pos.y);
			pos_node.append_attribute("z").set_value(pos.z);
			pugi::xml_node rot_node = transform_node.append_child("rotation");
			glm::vec3 rot = model->get_rotation();
			rot_node.append_attribute("x").set_value(rot.x);
			rot_node.append_attribute("y").set_value(rot.y);
			rot_node.append_attribute("z").set_value(rot.z);
			pugi::xml_node scale_node = transform_node.append_child("scale");
			glm::vec3 scale = model->get_scale();
			scale_node.append_attribute("x").set_value(scale.x);
			scale_node.append_attribute("y").set_value(scale.y);
			scale_node.append_attribute("z").set_value(scale.z);
		}

		doc.save_file("data/scene.xml");
		HMK_PRINT("Scene saved.");
		return;
	}

	if(key == HMK_KEY_DELETE && selected_model_index != -1)
	{
		scene_models.erase(std::begin(scene_models) + selected_model_index, std::begin(scene_models) + selected_model_index + 1);
		selected_model_index = -1;
	}

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
		cameras_[0]->rotate((float)xOffset, (float)yOffset);

	if(mouse_left_pressed_)
	{
		int x = (int)xPos;
		int y = (int)yPos;
		if(!is_mouse_on_gui)
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

	for(unsigned int i = 0; i < model_names.size(); ++i)
	{
		filename = model_names[i];
		HMK_PRINT("Loading " + filename);
		hmk::ModelUPtr temp_model = std::make_unique<hmk::Model>();
		temp_model->load(filename);
		temp_model->set_name(file_proper_name);
		temp_model->set_position(glm::vec3(0.0f));
		scene_models.emplace_back(std::move(temp_model));
		HMK_PRINT("Done " + filename);
	}
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
