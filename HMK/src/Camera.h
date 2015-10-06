#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

namespace hmk
{

const float MAX_PITCH = 89.0f;

class Camera
{
public:
	Camera();
	Camera(int width, int height);
	~Camera();

	void create_look_at(const glm::vec3 &pos, const glm::vec3 &up = glm::vec3(0.0f, 1.0f, 0.0f));
	void create_perspective_proj(float fovY, float nearZ, float farZ);
	void create_orthographic_proj(float left, float right, float top, float bottom, float nearZ, float farZ);

	glm::mat4 get_view_matrix();
	glm::mat4 get_proj_matrix();
	glm::vec3 get_position();

	void move_forward(float dt);
	void move_backward(float dt);
	void move_left(float dt);
	void move_right(float dt);
	void rotate(float x, float y);

	void set_sensitivity(float s);
	void set_camera_speed(float s);

private:
	void update_camera_vectors();

private:
	glm::mat4 projection_matrix_;
	glm::vec3 position_, world_up_;
	glm::vec3 front_, right_, up_;
	int width_, height_;
	float pitch_, yaw_;
	float sensitivity_;
	float movement_speed_;
};

typedef std::shared_ptr<Camera> CameraPtr;
}