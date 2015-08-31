#pragma once
#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include "Material.h"
#include "Texture.h"

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

private:
	GLuint mVAO, mVBO, mIBO;
	Material mMaterial;
	unsigned int mIndicesSize;

	std::shared_ptr<Texture> mAlbedoTexture, mNormalTexture, mRoughnessTexture, mMetalnessTexture;
};
}