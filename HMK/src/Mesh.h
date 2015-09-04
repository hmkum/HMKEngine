#pragma once
#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>
#include <memory>
#include <vector>
#include "Material.h"
#include "Texture.h"
#include "ShaderProgram.h"

namespace hmk
{

struct Vertex
{
	glm::vec3 Position;
	glm::vec2 TexCoord0;
	glm::vec3 Normal;
	glm::vec3 Tangent;
};

class Mesh
{
public:
	Mesh(std::vector<Vertex> vertices, Material &material, std::vector<unsigned int> indices);
	~Mesh();
	void Render();
	void Render(ShaderProgram &shader);

	void SetRoughness(float r);
	float GetRoughness();

	void SetMetallic(float m);
	float GetMetallic();

private:
	GLuint mVAO, mVBO, mIBO;
	Material mMaterial;
	MaterialUniform mMaterialUniform;
	unsigned int mIndicesSize;
	std::shared_ptr<Texture> mAlbedoTexture, mNormalTexture, mRoughnessTexture, mMetalnessTexture;
};
}