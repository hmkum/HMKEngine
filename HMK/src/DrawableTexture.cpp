#include "DrawableTexture.h"

using namespace hmk;

DrawableTexture::DrawableTexture()
	: fbo_id_{0}
	, depth_map_{0}
	, color_map_{0}
	, width_{1024}
	, height_{1024}
{
}

DrawableTexture::~DrawableTexture()
{
	glDeleteFramebuffers(1, &fbo_id_);
	glDeleteTextures(1, &depth_map_);
	glDeleteTextures(1, &color_map_);
}

bool DrawableTexture::initialize(bool hasColorMap, const GLuint width, const GLuint height, GLint format)
{
	width_ = width;
	height_ = height;

	glGenFramebuffers(1, &fbo_id_);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_id_);

	glGenTextures(1, &depth_map_);
	glBindTexture(GL_TEXTURE_2D, depth_map_);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width_, height_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map_, 0);
	glDrawBuffer(GL_NONE);
	//glReadBuffer(GL_NONE);

	if(hasColorMap)
	{
		glGenTextures(1, &color_map_);
		glBindTexture(GL_TEXTURE_2D, color_map_);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width_, height_, 0, GL_RGBA, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glGenerateMipmap(GL_TEXTURE_2D);

		GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, DrawBuffers);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_map_, 0);
	}

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		HMK_PRINT("Framebuffer tamamlanamadi.");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

void DrawableTexture::bind()
{
	glViewport(0, 0, width_, height_);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_id_);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void DrawableTexture::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
