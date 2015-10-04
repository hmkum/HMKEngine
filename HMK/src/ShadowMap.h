#pragma once
#include <GL/gl3w.h>

#include <memory>

namespace hmk
{
class ShadowMap
{
public:
	ShadowMap();
	~ShadowMap();

	bool Init(const GLuint width = 1024, const GLuint height = 1024);
	void Bind();
	void Unbind();

	GLuint GetDepthMap() const;
private:
	GLuint mFBO;
	GLuint mDepthMap;
	GLuint mWidth, mHeight;

};

typedef std::shared_ptr<ShadowMap> ShadowMapPtr;
}