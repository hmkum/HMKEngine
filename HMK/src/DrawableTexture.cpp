#include "DrawableTexture.h"

using namespace hmk;

DrawableTexture::DrawableTexture()
{
	mFBO = mRBO = 0;
	mDepthMap = 0;
	mColorMap = 0;
	mWidth = 1024;
	mHeight = 1024;
}

DrawableTexture::~DrawableTexture()
{
	glDeleteFramebuffers(1, &mFBO);
}

bool DrawableTexture::Init(bool hasColorMap, const GLuint width, const GLuint height, GLint format)
{
	mWidth = width;
	mHeight = height;

	glGenFramebuffers(1, &mFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

	glGenRenderbuffers(1, &mRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, mRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mWidth, mHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRBO);

	if(hasColorMap)
	{
		glGenTextures(1, &mColorMap);
		glBindTexture(GL_TEXTURE_2D, mColorMap);
		glTexImage2D(GL_TEXTURE_2D, 0, format, mWidth, mHeight, 0, GL_RGBA, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, DrawBuffers);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorMap, 0);
	}

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		HMK_PRINT("Framebuffer tamamlanamadi.");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

void DrawableTexture::Bind()
{
	glViewport(0, 0, mWidth, mHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void DrawableTexture::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint DrawableTexture::GetColorMap() const
{
	return mColorMap;
}

GLuint DrawableTexture::GetDepthMap() const
{
	return mDepthMap;
}