#pragma once
#include <GL/gl3w.h>
#include <memory>
#include "AlignedAllocation.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "DrawableTexture.h"

namespace hmk
{
class PostProcess : public AlignedAllocation<16>
{
public:
	PostProcess();
	~PostProcess();

	bool initialize();
	void begin();
	void end();
	void render(ShaderProgramSPtr shader);

	void do_monochrome();
	void do_hdr(float exposure = 1.0f, bool isBloomActive = true);
	void do_motion_blur(const glm::mat4 &viewProjMatrix);
	void do_negative();
	void do_blur();
	void do_down_filter(int level);
	void do_bright_pass();
	void do_bloom(float intensity);

private:
	DrawableTexture default_;
	DrawableTexture monochrome_, hdr_, motion_blur_, negative_, blurs_[2];
	DrawableTexture down_filter_, bright_pass_, bloom_;
	ShaderProgram shader_monochrome_, shader_hdr_, shader_motion_blur_, shader_negative_, shader_blurs_[2];
	ShaderProgram shader_down_filter_, shader_bright_pass_, shader_bloom_;
private:
	glm::mat4 previous_view_projection_matrix_;

private:
	GLuint vao_id_, vbo_id_;
	GLuint last_color_map_;
	GLuint bloom_color_map_;
	float gui_bloom_intensity_;
};

using PostProcessSPtr = std::shared_ptr<PostProcess>;
using PostProcessUPtr = std::unique_ptr<PostProcess>;
}