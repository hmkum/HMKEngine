#include "Game.h"
#include <imgui/imgui.h>
#include "KeyManager.h"
#include "Utility.h"

Game::Game()
{
	mMouseRightPressed = false;
	mLightPosition = glm::vec3(0, 0, 4);
}

Game::~Game()
{
}

bool Game::Init()
{
	mCamera = std::make_shared<hmk::Camera>(800, 600);
	mCamera->CreateLookAt(glm::vec3(0.0f, 0.0f, 5.0f));
	mCamera->CreatePerspectiveProj(120.0f, 0.1f, 100.0f);

	hmk::Shader vert, frag;
	vert.Init(GL_VERTEX_SHADER, "default.vert");
	frag.Init(GL_FRAGMENT_SHADER, "default.frag");
	mBasicShader.AddShader(vert).AddShader(frag).Link();
	
	hmk::Shader vert2, frag2;
	vert2.Init(GL_VERTEX_SHADER, "skybox.vert");
	frag2.Init(GL_FRAGMENT_SHADER, "skybox.frag");
	mSkyboxShader.AddShader(vert2).AddShader(frag2).Link();
	
	mSphere = std::make_shared<hmk::Model>();
	mSphere->Load("sphere.obj");
	//mSphere->Translate(glm::vec3(-5.0f, 0.0f, 0.0f));
	//mSphere->Scale(glm::vec3(0.1f));
	//mSphere->Rotate(180.0f, glm::vec3(0, 1, 0));

	//mAxe = std::make_shared<hmk::Model>();
	//mAxe->Load("axe.obj");
	//mAxe->Scale(glm::vec3(0.1f));
	//mAxe->Rotate(90.0f, glm::vec3(1, 0, 0));
	//mAxe->Rotate(180.0f, glm::vec3(0, 0, 1));

	mSkybox = std::make_shared<hmk::Skybox>();
	mSkybox->Load("Bridge/");

	return true;
}

void Game::Update(float dt)
{
	if (hmk::KeyManager::GetKey(HMK_KEY_W))
		mCamera->MoveForward(dt);
	if (hmk::KeyManager::GetKey(HMK_KEY_S))
		mCamera->MoveBackward(dt);
	if (hmk::KeyManager::GetKey(HMK_KEY_A))
		mCamera->MoveLeft(dt);
	if (hmk::KeyManager::GetKey(HMK_KEY_D))
		mCamera->MoveRight(dt);

	static float r = 0.0f, m = 0.0f;
	ImGui::Begin("Material");
	ImGui::SliderFloat("Roughness", &r, 0.0f, 1.0f);
	ImGui::SliderFloat("Metallic", &m, 0.0f, 1.0f);
	ImGui::Separator();
	ImGui::DragFloat3("Light Position", (float*)&mLightPosition.x, 0.1f);
	ImGui::End();
	mSphere->SetRoughness(r);
	mSphere->SetMetallic(m);
	
	//mAxe->Rotate(35.0f * dt, glm::vec3(0, 0, 1));
}

void Game::Render()
{	
	mSkyboxShader.Use();
	glm::mat4 skyboxModelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(40.0f));
	mSkyboxShader.SetUniform("uModel", skyboxModelMatrix);
	mSkyboxShader.SetUniform("uViewMatrix", mCamera->GetViewMatrix());
	mSkyboxShader.SetUniform("uProjMatrix", mCamera->GetProjMatrix());
	mSkybox->Render();

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mSkybox->GetTextureID());

	mBasicShader.Use();
	mBasicShader.SetUniform("uModel", mSphere->GetModelMatrix());
	mBasicShader.SetUniform("uViewMatrix", mCamera->GetViewMatrix());
	mBasicShader.SetUniform("uProjMatrix", mCamera->GetProjMatrix());
	mBasicShader.SetUniform("uCameraPosition", mCamera->GetPosition());
	mBasicShader.SetUniform("uLightPosition", mLightPosition);
	mSphere->Render(mBasicShader);
	
	//mBasicShader.SetUniform("uModel", mAxe->GetModelMatrix());
	//mAxe->Render(mBasicShader);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Game::KeyInput(int key, int scancode, int action, int mods)
{
	hmk::KeyManager::SetKey(key, (action == HMK_RELEASE) ? false : true);
	if(key == HMK_KEY_ESCAPE)
	{
		exit(1);
	}
}

void Game::CursorPosInput(double xPos, double yPos)
{
	if(mCursorState.firstMouse)
	{
		mCursorState.last = glm::vec2(xPos, yPos);
		mCursorState.firstMouse = false;
	}

	double xOffset = xPos - mCursorState.last.x;
	double yOffset = mCursorState.last.y - yPos;
	mCursorState.last = glm::vec2(xPos, yPos);

	if(mMouseRightPressed)
		mCamera->Rotate((float)xOffset, (float)yOffset);
}

void Game::MouseButtonInput(int button, int action, int mods)
{
	if (button == HMK_MOUSE_BUTTON_RIGHT && action == HMK_PRESS)
		mMouseRightPressed = true;
	else
		mMouseRightPressed = false;
}
