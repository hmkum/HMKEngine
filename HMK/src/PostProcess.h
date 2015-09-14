#pragma once
#include <GL/gl3w.h>
#include "Shader.h"
#include "ShaderProgram.h"
#include "DrawableTexture.h"

namespace hmk
{
class PostProcess
{
public:
	PostProcess();
	bool Init();
	void Begin();
	void End();
	void Render(ShaderProgram &shader);

	void DoMonochrome();
	void DoHDR(float exposure = 1.0f);
	void DoMotionBlur(const glm::mat4 &viewProjMatrix);
	void DoNegative();

private:
	DrawableTexture mDefault;
	DrawableTexture mMonochrome, mHDR, mMotionBlur, mNegative;

	ShaderProgram mMonochromeShader, mHDRShader, mMotionBlurShader, mNegativeShader;

private:
	glm::mat4 mPrevViewProjMatrix;

private:
	GLuint mVAO, mVBO;
	GLuint mLastColorMap;
};
}