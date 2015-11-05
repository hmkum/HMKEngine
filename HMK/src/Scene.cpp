#include <imgui/imgui.h>
#include <pugixml/pugixml.hpp>
#include "KeyManager.h"
#include "Keys.h"
#include "Ray.h"
#include "Scene.h"
#include "SceneParser.h"
#include "Utility.h"

using namespace hmk;

Scene::Scene()
{
	name_ = "";
	current_camera_index_ = 0;
	selected_model_index_ = -1;
	gui_is_shadow_active_ = true;
	gui_is_bloom_active_  = true;
	gui_bloom_intensity_  = 1.f;
	gui_tonemap_exposure_ = 1.f;
	gui_is_motionblur_active_ = true;
	gui_is_monochrome_active_ = false;
	gui_is_negative_active_   = false;
	is_mouse_on_gui_		  = false;
	mouse_left_pressed_		  = false;
	mouse_right_pressed_	  = false;
}

Scene::~Scene()
{
}

std::string Scene::initialize(std::string scene_file)
{
	std::string scene_parse_result = SceneParser::parse(scene_file);
	if(scene_parse_result != "")
	{
		HMK_LOG_ERROR("Could not parse scene.xml. Error: ", scene_parse_result)
		return scene_parse_result;
	}
	hmk::SceneData scene_data = hmk::SceneParser::get_data();
	name_ = scene_data.name_;

	for(const auto camera_data : scene_data.cameras_)
	{
		auto camera = std::make_shared<hmk::Camera>();
		camera->create_look_at(camera_data.position_);
		if(camera_data.projection_ == "perspective")
			camera->create_perspective_proj(camera_data.fov_, camera_data.near_z_, camera_data.far_z_);
		else
			camera->create_orthographic_proj(camera_data.ortho_params_, camera_data.near_z_, camera_data.far_z_);

		camera->rotate(camera_data.yaw_ + 90.f * 4, camera_data.pitch_);
		add_camera(camera);
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
		add_entity(temp_model);
		HMK_PRINT("Done: " + model.file_);
	}

	skybox_ = std::make_unique<hmk::Skybox>();
	skybox_->load(scene_data.atmosphere_.skybox_folder_);

	shadow_map_ = std::make_unique<ShadowMap>();
	if(shadow_map_->initialize(2048, 2048) == false)
	{
		return "Could not initialize shadow map!";
	}

	post_process_system_ = std::make_unique<PostProcess>();
	if(post_process_system_->initialize() == false)
	{
		return "Could not initialize post process system!";
	}
	
	ShaderProgramSPtr shader1 = std::make_shared<ShaderProgram>();
	shader1->add_shader("default.vert", "default.frag");
	shader1->link_shaders();
	shaders_.push_back(shader1);

	ShaderProgramSPtr shader2 = std::make_shared<ShaderProgram>();
	shader2->add_shader("skybox.vert", "skybox.frag");
	shader2->link_shaders();
	shaders_.push_back(shader2);

	ShaderProgramSPtr shader3 = std::make_shared<ShaderProgram>();
	shader3->add_shader("simple_depth.vert", "simple_depth.frag");
	shader3->link_shaders();
	shaders_.push_back(shader3);

	ShaderProgramSPtr shader4 = std::make_shared<ShaderProgram>();
	shader4->add_shader("post_process.vert", "post_process.frag");
	shader4->link_shaders();
	shaders_.push_back(shader4);

	sun_ = std::make_shared<DirectionalLight>();
	sun_->set_position(glm::vec3(0, 4, 4));
	sun_->set_target(glm::vec3(0, 1, -5));
	sun_->set_color(glm::vec3(1.f, 0.9762f, 0.86f));
	
	return "";
}

void Scene::update(float dt)
{
	if(hmk::KeyManager::get_key(HMK_KEY_W))
		cameras_[current_camera_index_]->move_forward(dt);
	if(hmk::KeyManager::get_key(HMK_KEY_S))
		cameras_[current_camera_index_]->move_backward(dt);
	if(hmk::KeyManager::get_key(HMK_KEY_A))
		cameras_[current_camera_index_]->move_left(dt);
	if(hmk::KeyManager::get_key(HMK_KEY_D))
		cameras_[current_camera_index_]->move_right(dt);

	light_position_ = sun_->get_target();
	ImGui::Begin("Properties");
	if(ImGui::CollapsingHeader("World Properties"))
	{
		ImGui::DragFloat3("Light Position", (float*)&light_position_.x, 0.1f);
		sun_->set_target(light_position_);
		ImGui::Checkbox("Shadows", &gui_is_shadow_active_);
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
	if(selected_model_index_ != -1)
	{
		if(ImGui::CollapsingHeader("Entity Properties"))
		{
			auto entity = entities_[selected_model_index_];
			r = DYN_PTR_CAST(Model, entity)->get_roughness();
			m = DYN_PTR_CAST(Model, entity)->get_metallic();
			selected_model_position_ = DYN_PTR_CAST(Model, entity)->get_position();
			selected_model_rotation_ = DYN_PTR_CAST(Model, entity)->get_rotation();
			selected_model_scale_	 = DYN_PTR_CAST(Model, entity)->get_scale();

			std::string name = DYN_PTR_CAST(Model, entity)->get_name();
			ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), name.c_str());
			if(ImGui::SliderFloat("Roughness", &r, 0.0f, 1.0f))
				DYN_PTR_CAST(Model, entity)->set_roughness(r);
			if(ImGui::SliderFloat("Metallic", &m, 0.0f, 1.0f))
				DYN_PTR_CAST(Model, entity)->set_metallic(m);

			ImGui::Separator();

			if(ImGui::DragFloat3("Position", (float*)&selected_model_position_.x, 0.1f))
				DYN_PTR_CAST(Model, entity)->set_position(selected_model_position_);

			if(ImGui::DragFloat3("Rotation", (float*)&selected_model_rotation_.x, 0.1f))
				DYN_PTR_CAST(Model, entity)->set_rotation(selected_model_rotation_);

			if(ImGui::DragFloat3("Scale", (float*)&selected_model_scale_.x, 0.1f))
				DYN_PTR_CAST(Model, entity)->set_scale(selected_model_scale_);
		}
	}
	ImGui::End();

	is_mouse_on_gui_ = ImGui::IsMouseHoveringAnyWindow();
}

void Scene::render()
{
	if(gui_is_shadow_active_)
	{
		shadow_map_->bind();

		shaders_[(int)Shaders::SimpleDepth]->use();
		shaders_[(int)Shaders::SimpleDepth]->set_uniform("uLightSpaceMatrix", sun_->get_vp_matrix());

		for(size_t i = 0; i < entities_.size(); ++i)
		{
			const auto model = DYN_PTR_CAST(Model, entities_[i]);
			shaders_[(int)Shaders::SimpleDepth]->set_uniform("uModel", model->get_model_matrix());
			model->render();
		}

		shadow_map_->unbind();

		glViewport(0, 0, 800, 600);
	}

	post_process_system_->begin();
	{
		if(skybox_.get() != nullptr)
		{
			shaders_[(int)Shaders::Skybox]->use();
			glm::mat4 skyboxModelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(40.0f));
			shaders_[(int)Shaders::Skybox]->set_uniform("uModel", skyboxModelMatrix);
			shaders_[(int)Shaders::Skybox]->set_uniform("uViewMatrix", cameras_[0]->get_view_matrix());
			shaders_[(int)Shaders::Skybox]->set_uniform("uProjMatrix", cameras_[0]->get_proj_matrix());
			skybox_->render();
		}

		shaders_[(int)Shaders::Default]->use();

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_->get_texture_id());

		shaders_[(int)Shaders::Default]->set_uniform("uIsShadowMapActive", 0);
		if(gui_is_shadow_active_)
		{
			shaders_[(int)Shaders::Default]->set_uniform("uIsShadowMapActive", 1);
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, shadow_map_->get_depth_map());
		}

		shaders_[(int)Shaders::Default]->set_uniform("uViewMatrix", cameras_[0]->get_view_matrix());
		shaders_[(int)Shaders::Default]->set_uniform("uProjMatrix", cameras_[0]->get_proj_matrix());
		shaders_[(int)Shaders::Default]->set_uniform("uCameraPosition", cameras_[0]->get_position());
		shaders_[(int)Shaders::Default]->set_uniform("uLightPosition", sun_->get_position());
		shaders_[(int)Shaders::Default]->set_uniform("uLightTarget", sun_->get_target());
		shaders_[(int)Shaders::Default]->set_uniform("uLightColor", sun_->get_color());
		shaders_[(int)Shaders::Default]->set_uniform("uLightSpaceMatrix", sun_->get_vp_matrix());

		for(size_t i = 0; i < entities_.size(); ++i)
		{
			const std::shared_ptr<Model> model = std::dynamic_pointer_cast<Model>(entities_[i]);
			shaders_[(int)Shaders::Default]->set_uniform("uModel", model->get_model_matrix());
			model->render(shaders_[(int)Shaders::Default]);
		}

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
	post_process_system_->end();

	const glm::mat4 viewProjMatrix = cameras_[0]->get_proj_matrix() * cameras_[0]->get_view_matrix();

	if(gui_is_bloom_active_) post_process_system_->do_bloom(gui_bloom_intensity_);
	if(gui_is_motionblur_active_) post_process_system_->do_motion_blur(viewProjMatrix);
	if(gui_is_monochrome_active_) post_process_system_->do_monochrome();
	if(gui_is_negative_active_) post_process_system_->do_negative();
	post_process_system_->do_hdr(gui_tonemap_exposure_, gui_is_bloom_active_);
	post_process_system_->render(shaders_[(int)Shaders::PostProcess]);
}

void Scene::key_input(int key, int scancode, int action, int mods)
{
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
		for(size_t i = 0; i < entities_.size(); ++i)
		{
			ModelSPtr model = DYN_PTR_CAST(Model, entities_[i]);
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

	if(key == HMK_KEY_DELETE && selected_model_index_ != -1)
	{
		entities_.erase(std::begin(entities_) + selected_model_index_, std::begin(entities_) + selected_model_index_ + 1);
		selected_model_index_ = -1;
	}
}

void Scene::cursor_pos_input(double xPos, double yPos, float xOffset, float yOffset)
{
	if(mouse_right_pressed_)
	cameras_[0]->rotate((float)xOffset, (float)yOffset);

	if(mouse_left_pressed_)
	{
		int x = (int)xPos;
		int y = (int)yPos;
		if(!is_mouse_on_gui_)
			process_selection(x, y);
	}
}

void Scene::mouse_button_input(int button, int action, int mods)
{
	if(button == HMK_MOUSE_BUTTON_RIGHT && action == HMK_PRESS)
		mouse_right_pressed_ = true;
	else
		mouse_right_pressed_ = false;

	if(button == HMK_MOUSE_BUTTON_LEFT && action == HMK_PRESS)
		mouse_left_pressed_ = true;
	else
		mouse_left_pressed_ = false;
}

void Scene::add_entity(EntitySPtr entity)
{
	if(entity.get() != nullptr)
	{
		entities_.push_back(entity);
	}
}

void Scene::add_camera(CameraSPtr camera)
{
	if(camera.get() != nullptr)
	{
		cameras_.push_back(camera);
	}
}

void Scene::set_skybox(SkyboxUPtr skybox)
{
	skybox_ = std::move(skybox);
}

void Scene::process_selection(int x, int y)
{
	glm::vec3 rayNear = glm::unProject(glm::vec3(x, 600 - y, 0.0f), cameras_[0]->get_view_matrix(), cameras_[0]->get_proj_matrix(), glm::vec4(0, 0, 800, 600));
	glm::vec3 rayFar = glm::unProject(glm::vec3(x, 600 - y, 1.0f), cameras_[0]->get_view_matrix(), cameras_[0]->get_proj_matrix(), glm::vec4(0, 0, 800, 600));

	hmk::Ray ray(cameras_[0]->get_position(), normalize(rayFar - rayNear));
	for(size_t i = 0; i < entities_.size(); ++i)
	{
		ModelSPtr model = std::dynamic_pointer_cast<Model>(entities_[i]);
		hmk::BoundingBox box = model->get_bounding_box();
		if(ray.intersect_aabb(box))
		{
			selected_model_index_ = i;
		}
	}
}