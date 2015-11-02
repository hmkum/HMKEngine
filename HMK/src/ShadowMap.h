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

	inline GLuint get_depth_map() const { return depth_map_; }
private:
	GLuint fbo_id_;
	GLuint depth_map_;
	GLuint width_, height_;

};

using ShadowMapPtr = std::shared_ptr<ShadowMap>; // TODO_HMK: Delete this
using ShadowMapSPtr = std::shared_ptr<ShadowMap>;
using ShadowMapUPtr = std::unique_ptr<ShadowMap>;
}