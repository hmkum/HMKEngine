#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include "AlignedAllocation.h"
#include "Camera.h"
#include "Scene.h"

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
	bool compile_and_link_all_shaders();

private:
	std::vector<std::unique_ptr<hmk::Scene>> scenes_;
	bool scene_rendered_;
	int current_scene_index_;

private:
	struct CursorState_t
	{
		bool first_mouse_ = true;
		glm::vec2 last_position_ = glm::vec2(400.0f, 300.0f);
	} cursor_state_;
};
