#include "ShadowMap.h"

using namespace hmk;

ShadowMap::ShadowMap()
{
	mFBO = 0;
	mDepthMap = 0;
	mWidth = 1024;
	mHeight = 1024;
}

ShadowMap::~ShadowMap()
{
	glDeleteFramebuffers(1, &mFBO);
}

bool ShadowMap::Init(const GLuint width, const GLuint height)
{
	mWidth    = width;
	mHeight   = height;

	glGenFramebuffers(1, &mFBO);

	glGenTextures(1, &mDepthMap);
	glBindTexture(GL_TEXTURE_2D, mDepthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mWidth, mHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

void ShadowMap::Bind()
{
	glViewport(0, 0, mWidth, mHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void ShadowMap::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint ShadowMap::GetDepthMap() const
{
	return mDepthMap;
}