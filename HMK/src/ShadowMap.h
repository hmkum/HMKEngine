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

	bool initialize(const GLuint width = 1024, const GLuint height = 1024);
	void bind();
	void unbind();

	GLuint get_depth_map() const;
private:
	GLuint fbo_id_;
	GLuint depth_map_;
	GLuint width_, height_;

};

typedef std::shared_ptr<ShadowMap> ShadowMapPtr;
}