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
	void DoHDR();

private:
	DrawableTexture mDefault;
	DrawableTexture mMonochrome, mHDR;

	ShaderProgram mMonochromeShader, mHDRShader;

private:
	GLuint mVAO, mVBO;
	GLuint mLastColorMap;
};
}