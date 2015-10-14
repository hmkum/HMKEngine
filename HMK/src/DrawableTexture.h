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

	bool initialize(bool hasColorMap = false, const GLuint width = 1024, const GLuint height = 1024, GLint format = GL_RGBA);
	void bind();
	void unbind();

	inline GLuint get_depth_map() const { return depth_map_; };
	inline GLuint get_color_map() const { return color_map_; };
private:
	GLuint fbo_id_;
	GLuint depth_map_;
	GLuint color_map_;
	GLuint width_, height_;

};
}