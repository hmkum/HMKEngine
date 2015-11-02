#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include "AlignedAllocation.h"

namespace hmk
{

const float MAX_PITCH = 89.0f;

enum class CameraProjection { Perspective, Orthographic };

class Camera : public AlignedAllocation<16>
{
public:
	Camera();
	Camera(int width, int height);
	~Camera();

	void create_look_at(const glm::vec3 &pos, const glm::vec3 &target = glm::vec3(0.0f, 0.0f, -1.0f), const glm::vec3 &up = glm::vec3(0.0f, 1.0f, 0.0f));
	void create_perspective_proj(float fovY, float nearZ, float farZ);
	void create_orthographic_proj(float left, float right, float top, float bottom, float nearZ, float farZ);
	void create_orthographic_proj(const glm::vec4& ortho_params, float nearZ, float farZ);

	inline glm::mat4 get_view_matrix()  const { return glm::lookAt(position_, position_ + front_, up_); }
	inline glm::mat4 get_proj_matrix()  const { return projection_matrix_; }
	inline glm::vec3 get_position()     const { return position_; }
	inline glm::vec3 get_target()       const { return front_; };
	inline glm::vec3 get_up_vector()    const { return up_; }
	inline float get_fov()			    const { return fov_; }
	inline float get_near_z()		    const { return near_z_; }
	inline float get_far_z()		    const { return far_z_; }
	inline glm::vec4 get_ortho_params() const { return ortho_params_; }
	inline CameraProjection get_projection() const { return camera_projection_; }
	inline float get_pitch()			const { return pitch_ / sensitivity_; }
	inline float get_yaw()				const { return yaw_ / sensitivity_; }

	void move_forward(float dt);
	void move_backward(float dt);
	void move_left(float dt);
	void move_right(float dt);
	void rotate(float x, float y);

	void set_position(const glm::vec3& pos);
	void set_sensitivity(float s);
	void set_camera_speed(float s);
	void set_fov(float fov);

private:
	void update_camera_vectors();

private:
	glm::mat4 projection_matrix_;
	glm::vec3 position_, world_up_;
	glm::vec3 front_, right_, up_;
	CameraProjection camera_projection_;
	int width_, height_;
	float fov_, near_z_, far_z_;
	glm::vec4 ortho_params_;
	float pitch_, yaw_;
	float sensitivity_;
	float movement_speed_;
};

using CameraPtr = std::shared_ptr<Camera>; // TODO_HMK: Delete this.
using CameraSPtr = std::shared_ptr<Camera>;
using CameraUPtr = std::unique_ptr<Camera>;
}