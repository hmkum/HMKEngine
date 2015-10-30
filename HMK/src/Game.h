#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include "AlignedAllocation.h"
#include "BoundingBox.h"
#include "Camera.h"
#include "Keys.h"
#include "Model.h"
#include "Skybox.h"
#include "ShadowMap.h"
#include "PostProcess.h"
#include "Ray.h"

class Game : public AlignedAllocation<16>
{
public:
	Game();
	~Game();

	bool initialize();
	void key_input(int key, int scancode, int action, int mods);
	void cursor_pos_input(double xPos, double yPos);
	void mouse_button_input(int button, int action, int mods);
	void drop_files_callback(int number_of_files, const char** filenames);

	void update(float dt);
	void render();

private:
	void process_selection(int x, int y);
	bool compile_and_link_all_shaders();

private:
	std::vector<hmk::ModelSPtr> scene_models;
	int selected_model_index;
	hmk::SkyboxPtr skybox_;
	hmk::ShaderProgram shader_basic_, shader_skybox_, shader_simple_depth_, shader_picking_with_mouse_, shader_post_process_;
	std::vector<hmk::CameraPtr> cameras_;
    hmk::ShadowMapPtr shadow_map_;
    hmk::PostProcessPtr post_process_system_;
	glm::vec3 light_position_;
	glm::mat4 light_projection_;
	glm::mat4 light_space_matrix_;
	glm::vec3 selected_model_position_;
	glm::vec3 selected_model_rotation_;
	glm::vec3 selected_model_scale_;

	bool gui_is_shadow_active = true;
	float gui_tonemap_exposure_ = 1.0f, gui_bloom_intensity_ = 1.0f;
	bool gui_is_bloom_active_ = true;
	bool gui_is_motionblur_active_ = true;
	bool gui_is_monochrome_active_ = false;
	bool gui_is_negative_active_ = false;
	bool is_mouse_on_gui = false;

private:
	struct CursorState_t
	{
		bool first_mouse_ = true;
		glm::vec2 last_position_ = glm::vec2(400.0f, 300.0f);
	} cursor_state_;
	bool mouse_right_pressed_, mouse_left_pressed_;
};
