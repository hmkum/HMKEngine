#include "Camera.h"

using namespace hmk;

Camera::Camera()
	: position_{glm::vec3(0.0f, 0.0f, 3.0f)}
	, world_up_{glm::vec3(0.0f, 1.0f, 0.0f)}
	, front_{glm::vec3(0.0f, 0.0f, -1.0f)}
	, width_{800}
	, height_{600}
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

void Camera::create_look_at(const glm::vec3 &pos, const glm::vec3 &up)
{
	position_ = pos;
	world_up_  = up;
	front_    = glm::vec3(0.0f, 0.0f, -1.0f);
	update_camera_vectors();
}

void Camera::create_perspective_proj(float fovY, float nearZ, float farZ)
{
	projection_matrix_ = glm::perspective(fovY, ((float)(width_) / height_), nearZ, farZ);
}

void Camera::create_orthographic_proj(float left, float right, float top, float bottom, float nearZ, float farZ)
{
	projection_matrix_ = glm::ortho(left, right, bottom, top, nearZ, farZ);
}

glm::mat4 Camera::get_view_matrix()
{
	return glm::lookAt(position_, position_ + front_, up_);
}

glm::mat4 Camera::get_proj_matrix()
{
	return projection_matrix_;
}

glm::vec3 Camera::get_position()
{
	return position_;
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

void Camera::set_sensitivity(float s)
{
	sensitivity_ = s;
}

void Camera::set_camera_speed(float s)
{
	movement_speed_ = s;
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
