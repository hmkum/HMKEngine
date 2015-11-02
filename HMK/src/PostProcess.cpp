#include "PostProcess.h"

using namespace hmk;

PostProcess::PostProcess()
	: vao_id_{0}
	, vbo_id_{0}
	, last_color_map_{0}
	, previous_view_projection_matrix_{1.0f}
{}

PostProcess::~PostProcess()
{
	glDeleteBuffers(1, &vbo_id_);
	glDeleteVertexArrays(1, &vao_id_);
}

bool PostProcess::initialize()
{
	GLfloat vertices[] = {
		// Back face
		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
		 1.0f,  1.0f, -1.0f, 1.0f, 1.0f, // top-right
		 1.0f, -1.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
		 1.0f,  1.0f, -1.0f, 1.0f, 1.0f,  // top-right
		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f,  // bottom-left
		-1.0f,  1.0f, -1.0f, 0.0f, 1.0f,// top-left
		// Front face
		-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, // bottom-left
		 1.0f, -1.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
		 1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  // top-right
		 1.0f,  1.0f, 1.0f, 1.0f, 1.0f, // top-right
		-1.0f,  1.0f, 1.0f, 0.0f, 1.0f,  // top-left
		-1.0f, -1.0f, 1.0f, 0.0f, 0.0f,  // bottom-left
		// Left face
		-1.0f,  1.0f,  1.0f, 1.0f, 0.0f, // top-right
		-1.0f,  1.0f, -1.0f, 1.0f, 1.0f, // top-left
		-1.0f, -1.0f, -1.0f, 0.0f, 1.0f,  // bottom-left
		-1.0f, -1.0f, -1.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f,  1.0f, 0.0f, 0.0f,  // bottom-right
		-1.0f,  1.0f,  1.0f, 1.0f, 0.0f, // top-right
		// Right face
		1.0f,  1.0f,  1.0f, 1.0f, 0.0f, // top-left
		1.0f, -1.0f, -1.0f, 0.0f, 1.0f, // bottom-right
		1.0f,  1.0f, -1.0f, 1.0f, 1.0f, // top-right         
		1.0f, -1.0f, -1.0f, 0.0f, 1.0f,  // bottom-right
		1.0f,  1.0f,  1.0f, 1.0f, 0.0f,  // top-left
		1.0f, -1.0f,  1.0f, 0.0f, 0.0f, // bottom-left     
		// Bottom face
		-1.0f, -1.0f, -1.0f, 0.0f, 1.0f, // top-right
		 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, // top-left
		 1.0f, -1.0f,  1.0f, 1.0f, 0.0f,// bottom-left
		 1.0f, -1.0f,  1.0f, 1.0f, 0.0f, // bottom-left
		-1.0f, -1.0f,  1.0f, 0.0f, 0.0f, // bottom-right
		-1.0f, -1.0f, -1.0f, 0.0f, 1.0f, // top-right
		// Top face
		-1.0f, 1.0f, -1.0f, 0.0f, 1.0f,// top-left
		 1.0f, 1.0f,  1.0f, 1.0f, 0.0f, // bottom-right
		 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, // top-right     
		 1.0f, 1.0f,  1.0f, 1.0f, 0.0f, // bottom-right
		-1.0f, 1.0f, -1.0f, 0.0f, 1.0f,// top-left
		-1.0f, 1.0f,  1.0f, 0.0f, 0.0f // bottom-left        
	};

	glGenVertexArrays(1, &vao_id_);
	glGenBuffers(1, &vbo_id_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_id_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(vao_id_);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glBindVertexArray(0);

	shader_monochrome_.add_shader("pp_default.vert", "pp_monochrome.frag");
	shader_monochrome_.link_shaders();
	
	shader_hdr_.add_shader("pp_default.vert", "pp_hdr.frag");
	shader_hdr_.link_shaders();

	shader_motion_blur_.add_shader("pp_default.vert", "pp_motion_blur.frag");
	shader_motion_blur_.link_shaders();

	shader_negative_.add_shader("pp_default.vert", "pp_negative.frag");
	shader_negative_.link_shaders();

	shader_blurs_[0].add_shader("pp_default.vert", "pp_blur_v.frag");
	shader_blurs_[0].link_shaders();

	shader_blurs_[1].add_shader("pp_default.vert", "pp_blur_h.frag");
	shader_blurs_[1].link_shaders();
	
	shader_down_filter_.add_shader("pp_default.vert", "pp_down_filter.frag");
	shader_down_filter_.link_shaders();

	shader_bloom_.add_shader("pp_default.vert", "pp_bloom.frag");
	shader_bloom_.link_shaders();

	shader_bright_pass_.add_shader("pp_default.vert", "pp_bright_pass.frag");
	shader_bright_pass_.link_shaders();

	bool result = true;
	result &= default_.initialize(true, 800, 600, GL_RGBA16F);
	result &= monochrome_.initialize(true, 800, 600, GL_RGBA16F);
	result &= hdr_.initialize(true, 800, 600);
	result &= motion_blur_.initialize(true, 800, 600, GL_RGBA16F);
	result &= negative_.initialize(true, 800, 600);
	result &= blurs_[0].initialize(true, 800, 600, GL_RGBA16F);
	result &= blurs_[1].initialize(true, 800, 600, GL_RGBA16F);
	result &= down_filter_.initialize(true, 800, 600, GL_RGBA16F);
	result &= bright_pass_.initialize(true, 800, 600, GL_RGBA16F);
	result &= bloom_.initialize(true, 800, 600, GL_RGBA16F);

	return result;
}

void PostProcess::begin()
{
	default_.bind();
}

void PostProcess::end()
{
	default_.unbind();
	last_color_map_ = default_.get_color_map();
}

void PostProcess::render(ShaderProgramSPtr shader)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, last_color_map_);
	glBindVertexArray(vao_id_);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void PostProcess::do_monochrome()
{
	monochrome_.bind();
	shader_monochrome_.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, last_color_map_);
	glBindVertexArray(vao_id_);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	last_color_map_ = monochrome_.get_color_map();
	monochrome_.unbind();
}

void PostProcess::do_hdr(float exposure, bool isBloomActive)
{
	hdr_.bind();
	shader_hdr_.use();
	shader_hdr_.set_uniform("uExposure", exposure);
	shader_hdr_.set_uniform("uIsBloomActive", isBloomActive);
	shader_hdr_.set_uniform("uBloomIntensity", gui_bloom_intensity_);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, last_color_map_);
	if(isBloomActive)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, bloom_color_map_);
	}

	glBindVertexArray(vao_id_);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	last_color_map_ = hdr_.get_color_map();
	hdr_.unbind();
}

void PostProcess::do_motion_blur(const glm::mat4 &viewProjMatrix)
{
	const glm::mat4 invMatrix = glm::inverse(viewProjMatrix);

	motion_blur_.bind();
	shader_motion_blur_.use();
	shader_motion_blur_.set_uniform("uInvViewProj", invMatrix);
	shader_motion_blur_.set_uniform("uPrevViewProj", previous_view_projection_matrix_);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, last_color_map_);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, default_.get_depth_map());
	glBindVertexArray(vao_id_);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	last_color_map_ = motion_blur_.get_color_map();
	motion_blur_.unbind();

	previous_view_projection_matrix_ = viewProjMatrix;
}

void PostProcess::do_negative()
{
	negative_.bind();
	shader_negative_.use();
	shader_negative_.set_uniform("uIsGrayScale", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, last_color_map_);
	glBindVertexArray(vao_id_);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	last_color_map_ = negative_.get_color_map();
	negative_.unbind();
}

void PostProcess::do_blur()
{
	blurs_[0].bind();
	shader_blurs_[0].use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, last_color_map_);
	glBindVertexArray(vao_id_);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	last_color_map_ = blurs_[0].get_color_map();
	blurs_[0].unbind();

	blurs_[1].bind();
	shader_blurs_[1].use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, last_color_map_);
	glBindVertexArray(vao_id_);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	last_color_map_ = blurs_[1].get_color_map();
	blurs_[1].unbind();
}

void PostProcess::do_down_filter(int level)
{
	do_blur();

	down_filter_.bind();
	shader_down_filter_.use();
	shader_down_filter_.set_uniform("uFilterLevel", level);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, last_color_map_);
	glBindVertexArray(vao_id_);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	last_color_map_ = down_filter_.get_color_map();
	down_filter_.unbind();

	do_blur();
}

void PostProcess::do_bright_pass()
{
	bright_pass_.bind();
	shader_bright_pass_.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, last_color_map_);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindVertexArray(vao_id_);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	last_color_map_ = bright_pass_.get_color_map();
	bright_pass_.unbind();
}

void PostProcess::do_bloom(float intensity)
{
	gui_bloom_intensity_ = intensity;
	GLuint tmpColorMap = last_color_map_;
	do_bright_pass();
	do_blur();

	bloom_color_map_ = last_color_map_;
	last_color_map_ = tmpColorMap;
}