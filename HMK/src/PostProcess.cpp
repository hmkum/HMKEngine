#include "PostProcess.h"

using namespace hmk;

PostProcess::PostProcess()
{
	mVAO = mVBO = 0;
	mLastColorMap = 0;
}

bool PostProcess::Init()
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

	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(mVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glBindVertexArray(0);

	hmk::Shader vert, frag;
	vert.Init(GL_VERTEX_SHADER, "pp_default.vert");
	frag.Init(GL_FRAGMENT_SHADER, "pp_monochrome.frag");
	mMonochromeShader.AddShader(vert).AddShader(frag).Link();

	frag.Init(GL_FRAGMENT_SHADER, "pp_hdr.frag");
	mHDRShader.AddShader(vert).AddShader(frag).Link();

	bool result = false;
	result &= mDefault.Init(true, 800, 600, GL_RGBA16F);
	result &= mMonochrome.Init(true, 800, 600);
	result &= mHDR.Init(true, 800, 600);

	return result;
}

void PostProcess::Begin()
{
	mDefault.Bind();
}

void PostProcess::End()
{
	mDefault.Unbind();
	mLastColorMap = mDefault.GetColorMap();
}

void PostProcess::Render(ShaderProgram &shader)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader.Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mLastColorMap);
	glBindVertexArray(mVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void PostProcess::DoMonochrome()
{
	mMonochrome.Bind();
	mMonochromeShader.Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mLastColorMap);
	glBindVertexArray(mVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	mLastColorMap = mMonochrome.GetColorMap();
	mMonochrome.Unbind();
}

void PostProcess::DoHDR()
{
	mHDR.Bind();
	mHDRShader.Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mLastColorMap);
	glBindVertexArray(mVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	mLastColorMap = mHDR.GetColorMap();
	mHDR.Unbind();
}

