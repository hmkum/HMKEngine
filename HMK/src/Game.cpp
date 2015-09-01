#include "Game.h"
#include "KeyManager.h"
#include "Utility.h"

Game::Game()
{
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
	mSphere->Translate(glm::vec3(-5.0f, 0.0f, 0.0f));
	//mSphere->Scale(glm::vec3(0.1f));
	//mSphere->Rotate(180.0f, glm::vec3(0, 1, 0));

	mAxe = std::make_shared<hmk::Model>();
	mAxe->Load("axe.obj");
	mAxe->Scale(glm::vec3(0.1f));
	mAxe->Rotate(90.0f, glm::vec3(1, 0, 0));
	mAxe->Rotate(180.0f, glm::vec3(0, 0, 1));

	mSkybox = std::make_shared<hmk::Skybox>();
	mSkybox->Load("LancellottiChapel/");

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

	if (hmk::KeyManager::GetKey(HMK_KEY_KP_1))
	{
		mSphere->SetRoughness(mSphere->GetRoughness() - dt);
		HMK_PRINT("Roughness: ", mSphere->GetRoughness());
	}
	if (hmk::KeyManager::GetKey(HMK_KEY_KP_3))
	{
		mSphere->SetRoughness(mSphere->GetRoughness() + dt);
		HMK_PRINT("Roughness: ", mSphere->GetRoughness());
	}
	if (hmk::KeyManager::GetKey(HMK_KEY_KP_4))
	{
		mSphere->SetMetallic(mSphere->GetMetallic() - dt);
		HMK_PRINT("Metallic: ", mSphere->GetMetallic());
	}
	if (hmk::KeyManager::GetKey(HMK_KEY_KP_6))
	{
		mSphere->SetMetallic(mSphere->GetMetallic() + dt);
		HMK_PRINT("Metallic: ", mSphere->GetMetallic());
	}
	
	mAxe->Rotate(35.0f * dt, glm::vec3(0, 0, 1));
}

void Game::Render()
{
	mBasicShader.Use();

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mSkybox->GetTextureID());

	mBasicShader.SetUniform("uModel", mSphere->GetModelMatrix());
	mBasicShader.SetUniform("uViewMatrix", mCamera->GetViewMatrix());
	mBasicShader.SetUniform("uProjMatrix", mCamera->GetProjMatrix());
	mBasicShader.SetUniform("uCameraPosition", mCamera->GetPosition());
	mSphere->Render(mBasicShader);
	
	mBasicShader.SetUniform("uModel", mAxe->GetModelMatrix());
	mAxe->Render(mBasicShader);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	mSkyboxShader.Use();
	glm::mat4 skyboxModelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(40.0f));
	mSkyboxShader.SetUniform("uModel", skyboxModelMatrix);
	mSkyboxShader.SetUniform("uViewMatrix", mCamera->GetViewMatrix());
	mSkyboxShader.SetUniform("uProjMatrix", mCamera->GetProjMatrix());
	mSkybox->Render();
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

	mCamera->Rotate((float)xOffset, (float)yOffset);
}

void Game::MouseButtonInput(int button, int action, int mods)
{
}
