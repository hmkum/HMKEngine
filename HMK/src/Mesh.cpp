#include "Mesh.h"

using namespace hmk;

Mesh::Mesh(std::vector<Vertex> vertices, Material &material, std::vector<unsigned int> indices)
{
	mAlbedoTexture = nullptr;
	mMetalnessTexture = nullptr;
	mMaterial	 = material;
	mIndicesSize = indices.size();

	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);

	glGenBuffers(1, &mVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices.front(), GL_STATIC_DRAW);
	
	glGenBuffers(1, &mIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices.front(), GL_STATIC_DRAW);

	GLintptr posOff = 0 * sizeof(float);
	GLintptr texOff = 3 * sizeof(float);
	GLintptr norOff = 5 * sizeof(float);
	GLintptr tanOff = 8 * sizeof(float);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)posOff);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)texOff);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)norOff);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)tanOff);

	glBindVertexArray(0);

	if(material.HasTextures.x)
	{
		mAlbedoTexture = std::make_shared<Texture>();
		mAlbedoTexture->Create(material.mAlbedoTexName);
	}

	if (material.HasTextures.y)
	{
		mNormalTexture = std::make_shared<Texture>();
		mNormalTexture->Create(material.mNormalTexName);
	}

	if (material.HasTextures.z)
	{
		mRoughnessTexture = std::make_shared<Texture>();
		mRoughnessTexture->Create(material.mRoughnessTexName);
	}

	if(material.HasTextures.w)
	{
		mMetalnessTexture = std::make_shared<Texture>();
		mMetalnessTexture->Create(material.mMetallicTexName);
	}
}

Mesh::~Mesh()
{
}

void Mesh::Render()
{
	glBindVertexArray(mVAO);
	
	if (mAlbedoTexture) mAlbedoTexture->Bind();
	if (mNormalTexture) mNormalTexture->Bind(1);
	if (mRoughnessTexture) mRoughnessTexture->Bind(2);
	if (mMetalnessTexture) mMetalnessTexture->Bind(3);
	
	glDrawElements(GL_TRIANGLES, mIndicesSize, GL_UNSIGNED_INT, 0);

	if (mAlbedoTexture)	mAlbedoTexture->Unbind();
	if (mNormalTexture) mNormalTexture->Unbind();
	if (mRoughnessTexture)	mRoughnessTexture->Unbind();
	if (mMetalnessTexture) mMetalnessTexture->Unbind();

	glBindVertexArray(0);
}