#include "Camera.h"

using namespace hmk;

Camera::Camera()
	: mPosition{glm::vec3(0.0f, 0.0f, 3.0f)}
	, mWorldUp{glm::vec3(0.0f, 1.0f, 0.0f)}
	, mFront{glm::vec3(0.0f, 0.0f, -1.0f)}
	, mWidth{800}
	, mHeight{600}
	, mPitch{0.0f}
	, mYaw{-90.0f}
	, mSensitivity{0.25f}
	, mMovementSpeed{4.0f}
{
	UpdateCameraVectors();
}

Camera::Camera(int width, int height)
	: mPosition{glm::vec3(0.0f, 0.0f, 3.0f)}
	, mWorldUp{glm::vec3(0.0f, 1.0f, 0.0f)}
	, mFront{glm::vec3(0.0f, 0.0f, -1.0f)}
	, mWidth{width}
	, mHeight{height}
	, mPitch{0.0f}
	, mYaw{-90.0f}
	, mSensitivity{0.25f}
	, mMovementSpeed{4.0f}
{
	UpdateCameraVectors();
}

Camera::~Camera()
{
}

void Camera::CreateLookAt(const glm::vec3 &pos, const glm::vec3 &up)
{
	mPosition = pos;
	mWorldUp  = up;
	mFront    = glm::vec3(0.0f, 0.0f, -1.0f);
	UpdateCameraVectors();
}

void Camera::CreatePerspectiveProj(float fovY, float nearZ, float farZ)
{
	mProjMatrix = glm::perspective(fovY, ((float)(mWidth) / mHeight), nearZ, farZ);
}

void Camera::CreateOrthographicProj(float left, float right, float top, float bottom, float nearZ, float farZ)
{
	mProjMatrix = glm::ortho(left, right, bottom, top, nearZ, farZ);
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(mPosition, mPosition + mFront, mUp);
}

glm::mat4 Camera::GetProjMatrix()
{
	return mProjMatrix;
}

glm::vec3 Camera::GetPosition()
{
	return mPosition;
}

void Camera::MoveForward(float dt)
{
	mPosition += mFront * dt * mMovementSpeed;
}

void Camera::MoveBackward(float dt)
{
	mPosition -= mFront * dt * mMovementSpeed;
}

void Camera::MoveLeft(float dt)
{
	mPosition -= mRight * dt * mMovementSpeed;
}

void Camera::MoveRight(float dt)
{
	mPosition += mRight * dt * mMovementSpeed;
}

void Camera::Rotate(float x, float y)
{
	mYaw   += x * mSensitivity;
	mPitch += y * mSensitivity;

	// Avoid gimbal lock
	if(mPitch > MAX_PITCH)
		mPitch = MAX_PITCH;
	if(mPitch < -MAX_PITCH)
		mPitch = -MAX_PITCH;
	UpdateCameraVectors();
}

void Camera::SetSensitivity(float s)
{
	mSensitivity = s;
}

void Camera::SetCameraSpeed(float s)
{
	mMovementSpeed = s;
}

void Camera::UpdateCameraVectors()
{
	glm::vec3 front;
	front.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
	front.y = sin(glm::radians(mPitch));
	front.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
	mFront = glm::normalize(front);
	mRight = glm::normalize(glm::cross(mFront, mWorldUp));
	mUp = glm::normalize(glm::cross(mRight, mFront));
}
