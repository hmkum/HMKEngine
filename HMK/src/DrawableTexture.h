#pragma once
#include <GL/gl3w.h>
#include "Utility.h"

namespace hmk
{
class DrawableTexture
{
public:
	DrawableTexture();
	~DrawableTexture();
	bool Init(bool hasColorMap = false, const GLuint width = 1024, const GLuint height = 1024, GLint format = GL_RGBA);
	void Bind();
	void Unbind();

	GLuint GetDepthMap() const;
	GLuint GetColorMap() const;
private:
	GLuint mFBO;
	GLuint mDepthMap;
	GLuint mColorMap;
	GLuint mWidth, mHeight;

};
}