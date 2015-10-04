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

	void CreateLookAt(const glm::vec3 &pos, const glm::vec3 &up = glm::vec3(0.0f, 1.0f, 0.0f));
	void CreatePerspectiveProj(float fovY, float nearZ, float farZ);
	void CreateOrthographicProj(float left, float right, float top, float bottom, float nearZ, float farZ);

	glm::mat4 GetViewMatrix();
	glm::mat4 GetProjMatrix();
	glm::vec3 GetPosition();

	void MoveForward(float dt);
	void MoveBackward(float dt);
	void MoveLeft(float dt);
	void MoveRight(float dt);
	void Rotate(float x, float y);

	void SetSensitivity(float s);
	void SetCameraSpeed(float s);

private:
	void UpdateCameraVectors();

private:
	glm::mat4 mProjMatrix;
	glm::vec3 mPosition, mWorldUp;
	glm::vec3 mFront, mRight, mUp;
	int mWidth, mHeight;
	float mPitch, mYaw;
	float mSensitivity;
	float mMovementSpeed;
};

typedef std::shared_ptr<Camera> CameraPtr;
}