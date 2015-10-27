#include "Camera.h"

using namespace hmk;

Camera::Camera()
	: position_{glm::vec3(0.0f, 0.0f, 3.0f)}
	, world_up_{glm::vec3(0.0f, 1.0f, 0.0f)}
	, front_{glm::vec3(0.0f, 0.0f, -1.0f)}
	, width_{800}
	, height_{600}
	, fov_{120.f}
	, near_z_{0.1f}
	, far_z_{100.f}
	, ortho_params_{0.f, 1.f, 0.f, 1.f}
	, pitch_{0.0f}
	, yaw_{-90.0f}
	, sensitivity_{0.25f}
	, movement_speed_{4.0f}
{
	update_camera_vectors();
}

Camera::Camera(int width, int height)
	: position_{glm::vec3(0.0f, 0.0f, 3.0f)}
	, world_up_{glm::vec3(0.0f, 1.0f, 0.0f)}
	, front_{glm::vec3(0.0f, 0.0f, -1.0f)}
	, width_{width}
	, height_{height}
	, fov_{120.f}
	, near_z_{0.1f}
	, far_z_{100.f}
	, ortho_params_{0.f, 1.f, 0.f, 1.f}
	, pitch_{0.0f}
	, yaw_{-90.0f}
	, sensitivity_{0.25f}
	, movement_speed_{4.0f}
{
	update_camera_vectors();
}

Camera::~Camera()
{
}

void Camera::create_look_at(const glm::vec3 &pos,const glm::vec3& target, const glm::vec3 &up)
{
	position_ = pos;
	world_up_ = up;
	front_ = target;
	update_camera_vectors();
}

void Camera::create_perspective_proj(float fovY, float nearZ, float farZ)
{
	fov_    = fovY;
	near_z_ = nearZ;
	far_z_  = farZ;
	projection_matrix_ = glm::perspective(fovY, ((float)(width_) / height_), nearZ, farZ);
}

void Camera::create_orthographic_proj(float left, float right, float top, float bottom, float nearZ, float farZ)
{
	ortho_params_ = glm::vec4(left, right, top, bottom);
	near_z_ = nearZ;
	far_z_  = farZ;
	projection_matrix_ = glm::ortho(left, right, bottom, top, nearZ, farZ);
}

void Camera::create_orthographic_proj(const glm::vec4& ortho_params, float nearZ, float farZ)
{
	create_orthographic_proj(ortho_params.x, ortho_params.y, ortho_params.z, ortho_params.w, nearZ, farZ);
}

void Camera::move_forward(float dt)
{
	position_ += front_ * dt * movement_speed_;
}

void Camera::move_backward(float dt)
{
	position_ -= front_ * dt * movement_speed_;
}

void Camera::move_left(float dt)
{
	position_ -= right_ * dt * movement_speed_;
}

void Camera::move_right(float dt)
{
	position_ += right_ * dt * movement_speed_;
}

void Camera::rotate(float x, float y)
{
	yaw_   += x * sensitivity_;
	pitch_ += y * sensitivity_;

	// Avoid gimbal lock
	if(pitch_ > MAX_PITCH)
		pitch_ = MAX_PITCH;
	if(pitch_ < -MAX_PITCH)
		pitch_ = -MAX_PITCH;
	update_camera_vectors();
}

void Camera::set_position(const glm::vec3& pos)
{
	position_ = pos;
}

void Camera::set_sensitivity(float s)
{
	sensitivity_ = s;
}

void Camera::set_camera_speed(float s)
{
	movement_speed_ = s;
}

void Camera::set_fov(float fov)
{
	fov_ = fov;
}

void Camera::set_right_vector(const glm::vec3& right)
{
	right_ = right;
}


void Camera::update_camera_vectors()
{
	glm::vec3 front;
	front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
	front.y = sin(glm::radians(pitch_));
	front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
	front_ = glm::normalize(front);
	right_ = glm::normalize(glm::cross(front_, world_up_));
	up_ = glm::normalize(glm::cross(right_, front_));
}