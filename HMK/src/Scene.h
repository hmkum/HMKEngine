#pragma once
#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <memory>
#include <string>
#include "AlignedAllocation.h"
#include "Camera.h"
#include "Entity.h"
#include "Model.h"
#include "PostProcess.h"
#include "ShaderProgram.h"
#include "ShadowMap.h"
#include "Skybox.h"
namespace hmk
{
class Scene : public AlignedAllocation<16>
{
public:
	Scene();
	~Scene();

	std::string initialize();

	void add_entity(EntitySPtr entity);
	void add_camera(CameraSPtr camera);
	void set_skybox(SkyboxUPtr skybox);

	void update(float dt);
	void render();

	void key_input(int key, int scancode, int action, int mods);
	void cursor_pos_input(double xPos, double yPos, float xOffset, float yOffset);
	void mouse_button_input(int button, int action, int mods);
private:
	void process_selection(int x, int y);

private:
	std::vector<EntitySPtr> entities_;
	std::vector<CameraSPtr> cameras_;
	SkyboxUPtr skybox_;
	ShadowMapUPtr shadow_map_;
	PostProcessUPtr post_process_system_;
	std::vector<ShaderProgramSPtr> shaders_;

private:
	glm::vec3 light_position_;
	glm::mat4 light_projection_, light_space_matrix_;

	int current_camera_index_;
	int selected_model_index_;

	glm::vec3 selected_model_position_;
	glm::vec3 selected_model_rotation_;
	glm::vec3 selected_model_scale_;
private:
	bool gui_is_shadow_active_;
	bool gui_is_bloom_active_;
	float gui_bloom_intensity_, gui_tonemap_exposure_;
	bool gui_is_motionblur_active_;
	bool gui_is_monochrome_active_;
	bool gui_is_negative_active_;
	bool is_mouse_on_gui_;

	bool mouse_right_pressed_, mouse_left_pressed_;
};
}