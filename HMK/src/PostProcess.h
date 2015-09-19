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
	void DoHDR(float exposure = 1.0f, bool isBloomActive = true);
	void DoMotionBlur(const glm::mat4 &viewProjMatrix);
	void DoNegative();
	void DoGrayScale();
	void DoBlur();
	void DoDownFilter(int level);
	void DoBrightPass();
	void DoBloom(float intensity);

private:
	DrawableTexture mDefault;
	DrawableTexture mMonochrome, mHDR, mMotionBlur, mNegativeGrayscale, mBlur[2];
	DrawableTexture mDownFilter, mBrightPass, mBloom;
	ShaderProgram mMonochromeShader, mHDRShader, mMotionBlurShader, mNegativeGrayscaleShader, mBlurShader[2];
	ShaderProgram mDownFilterShader, mBrightPassShader, mBloomShader;
private:
	glm::mat4 mPrevViewProjMatrix;

private:
	GLuint mVAO, mVBO;
	GLuint mLastColorMap;
	GLuint mBloomColorMap;
	float mBloomIntensity;
};
}