#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include "AlignedAllocation.h"
#include "BoundingBox.h"
#include "Camera.h"
#include "Keys.h"
#include "Shader.h"
#include "ShaderProgram.h"
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

	bool Init();
	void KeyInput(int key, int scancode, int action, int mods);
	void CursorPosInput(double xPos, double yPos);
	void MouseButtonInput(int button, int action, int mods);

	void Update(float dt);
	void Render();

private:
	void ProcessSelection(int x, int y);

private:
	std::shared_ptr<hmk::Model> mSelectedModel;
	std::shared_ptr<hmk::Model> mAxe, mSphere, mSphere2, mPlane;
	std::shared_ptr<hmk::Skybox> mSkybox;
	hmk::ShaderProgram mBasicShader, mSkyboxShader, mSimpleDepthShader, mPickingShader, mPPShader;
	std::shared_ptr<hmk::Camera> mCamera;
	std::shared_ptr<hmk::ShadowMap> mShadowMap;
	std::shared_ptr<hmk::PostProcess> mPostProcess;
	glm::vec3 mLightPosition;
	glm::mat4 mLightProj;
	glm::mat4 mLightSpaceMatrix;

	float mTonemapExposure = 1.0f;

private:
	struct CursorState_t
	{
		bool firstMouse = true;
		glm::vec2 last = glm::vec2(400.0f, 300.0f);
	} mCursorState;
	bool mMouseRightPressed, mMouseLeftPressed;
};