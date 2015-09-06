#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "Model.h"
#include "Material.h"
#include "Utility.h"

using namespace hmk;

Model::Model()
{
	mDrawBoundingBox = false;
	mTranslation = glm::mat4(1.0f);
	mRotation	 = glm::mat4(1.0f);
	mScale		 = glm::mat4(1.0f);
}

Model::~Model()
{
}

bool Model::Load(std::string modelName)
{
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(MODEL_PATH + modelName, aiProcess_Triangulate | aiProcess_FlipUVs |
		aiProcess_CalcTangentSpace | aiProcess_FlipWindingOrder);

	if (!scene)
	{
		HMK_LOG_ERROR("Could not load model: ", modelName);
		return false;
	}

	// Handle scene
	unsigned int numMeshes = scene->mNumMeshes;
	for (unsigned int i = 0; i < numMeshes; ++i)
	{
		const aiMesh *mesh = scene->mMeshes[i];
		const aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

		// Handle mesh
		std::vector<unsigned int> indices;
		for (unsigned int j = 0; j < mesh->mNumFaces; ++j)
		{
			aiFace *face = &mesh->mFaces[j];
			indices.push_back(face->mIndices[0]);
			indices.push_back(face->mIndices[1]);
			indices.push_back(face->mIndices[2]);
		}

		std::vector<Vertex> vertices(mesh->mNumVertices);
		for (unsigned int j = 0; j < mesh->mNumVertices; ++j)
		{
			aiVector3D pos = mesh->mVertices[j];
			vertices[j].Position = glm::vec3(pos.x, pos.y, pos.z);

			mBoundingBox.mMin.x = min(mBoundingBox.mMin.x, pos.x);
			mBoundingBox.mMin.y = min(mBoundingBox.mMin.y, pos.y);
			mBoundingBox.mMin.z = min(mBoundingBox.mMin.z, pos.z);

			mBoundingBox.mMax.x = max(mBoundingBox.mMax.x, pos.x);
			mBoundingBox.mMax.y = max(mBoundingBox.mMax.y, pos.y);
			mBoundingBox.mMax.z = max(mBoundingBox.mMax.z, pos.z);

			aiVector3D one3D(1.0f, 1.0f, 1.0f);
			aiVector3D zero3D(0.0f, 0.0f, 0.0f);

			aiVector3D norm = mesh->HasNormals() ? mesh->mNormals[j] : one3D;
			vertices[j].Normal = glm::vec3(norm.x, norm.y, norm.z);

			aiVector3D texCoord = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][j] : zero3D;
			vertices[j].TexCoord0 = glm::vec2(texCoord.x, texCoord.y);

			aiVector3D tangent = mesh->HasTangentsAndBitangents() ? mesh->mTangents[j] : zero3D;
			vertices[j].Tangent = glm::vec3(tangent.x, tangent.y, tangent.z);
		}

		glGenVertexArrays(1, &mBBVAO);
		glBindVertexArray(mBBVAO);

		GLfloat bbVertices[] = {
			mBoundingBox.mMax.x, mBoundingBox.mMax.y, mBoundingBox.mMin.z,
			mBoundingBox.mMin.x, mBoundingBox.mMax.y, mBoundingBox.mMin.z,
			mBoundingBox.mMin.x, mBoundingBox.mMin.y, mBoundingBox.mMin.z,
			mBoundingBox.mMax.x, mBoundingBox.mMin.y, mBoundingBox.mMin.z,
			mBoundingBox.mMax.x, mBoundingBox.mMin.y, mBoundingBox.mMax.z,
			mBoundingBox.mMax.x, mBoundingBox.mMax.y, mBoundingBox.mMax.z,
			mBoundingBox.mMin.x, mBoundingBox.mMax.y, mBoundingBox.mMax.z,
			mBoundingBox.mMin.x, mBoundingBox.mMin.y, mBoundingBox.mMax.z,
			mBoundingBox.mMax.x, mBoundingBox.mMax.y, mBoundingBox.mMin.z,
			mBoundingBox.mMax.x, mBoundingBox.mMax.y, mBoundingBox.mMax.z,
			mBoundingBox.mMin.x, mBoundingBox.mMax.y, mBoundingBox.mMax.z,
			mBoundingBox.mMin.x, mBoundingBox.mMax.y, mBoundingBox.mMin.z,
			mBoundingBox.mMax.x, mBoundingBox.mMin.y, mBoundingBox.mMax.z,
			mBoundingBox.mMin.x, mBoundingBox.mMin.y, mBoundingBox.mMax.z,
			mBoundingBox.mMin.x, mBoundingBox.mMin.y, mBoundingBox.mMin.z,
			mBoundingBox.mMax.x, mBoundingBox.mMin.y, mBoundingBox.mMin.z
		};

		glGenBuffers(1, &mBBVBO);
		glBindBuffer(GL_ARRAY_BUFFER, mBBVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(bbVertices), bbVertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindVertexArray(0);

		// Handle material
		Material mat;
		aiColor4D color;
		if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == aiReturn_SUCCESS)
		{
			mat.mBaseColor = glm::vec3(color.r, color.g, color.b);
		}

		float value = 0.0f;
		if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			aiString filename;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &filename);
			mat.mAlbedoTexName = HandleTextureName(filename.C_Str());
			mat.mHasTextures.x = 1;
		}

		if (material->GetTextureCount(aiTextureType_HEIGHT) > 0) // Normal map
		{
			aiString filename;
			material->GetTexture(aiTextureType_HEIGHT, 0, &filename);
			mat.mNormalTexName = HandleTextureName(filename.C_Str());
			mat.mHasTextures.y = 1;
		}

		if (material->GetTextureCount(aiTextureType_SHININESS) > 0)
		{
			aiString filename;
			material->GetTexture(aiTextureType_SHININESS, 0, &filename);
			mat.mRoughnessTexName = HandleTextureName(filename.C_Str());
			mat.mHasTextures.z = 1;
		}

		if (material->GetTextureCount(aiTextureType_SPECULAR) > 0)
		{
			aiString filename;
			material->GetTexture(aiTextureType_SPECULAR, 0, &filename);
			mat.mMetallicTexName = HandleTextureName(filename.C_Str());
			mat.mHasTextures.w = 1;
		}

		// Initialize mesh
		std::shared_ptr<Mesh> mesh1 = std::make_shared<Mesh>(vertices, mat, indices);
		mMeshes.push_back(mesh1);
	}

	importer.FreeScene();
	return true;
}

void Model::Render()
{
	for (auto &mesh : mMeshes)
	{
		mesh->Render();
	}
}

void Model::Render(ShaderProgram &shader)
{
	if(mDrawBoundingBox)
	{
		glBindVertexArray(mBBVAO);
		glDrawArrays(GL_LINE_LOOP, 0, 16 * 3);
		glBindVertexArray(0);
	}

	for(auto &mesh : mMeshes)
	{
		mesh->Render(shader);
	}
}

glm::mat4 Model::GetModelMatrix() const
{
	return mScale * mRotation * mTranslation;
}

void Model::Translate(glm::vec3 t)
{
	mTranslation = glm::translate(mTranslation, t);
}

void Model::Rotate(float degree, glm::vec3 axis)
{
	mRotation = glm::rotate(mRotation, glm::radians(degree), axis);
}

void Model::Scale(glm::vec3 s)
{
	mScale = glm::scale(mScale, s);
}

void Model::SetRoughness(float r)
{
	if (r >= 1.0f) r = 1.0f;
	if (r <= 0.0f) r = 0.0f;
	for (auto &mesh : mMeshes)
	{
		mesh->SetRoughness(r);
	}
}

float Model::GetRoughness()
{
	return mMeshes[0]->GetRoughness();
}

void Model::SetMetallic(float m)
{
	if (m >= 1.0f) m = 1.0f;
	if (m <= 0.0f) m = 0.0f;
	for (auto &mesh : mMeshes)
	{
		mesh->SetMetallic(m);
	}
}

float Model::GetMetallic()
{
	return mMeshes[0]->GetMetallic();
}

void Model::DrawBoundingBox(bool draw)
{
	mDrawBoundingBox = draw;
}

BoundingBox Model::GetBoundingBox() const
{
	BoundingBox box;
	box.mMin = glm::vec3(GetModelMatrix() * glm::vec4(mBoundingBox.mMin, 1.0f));
	box.mMax = glm::vec3(GetModelMatrix() * glm::vec4(mBoundingBox.mMax, 1.0f));
	return box;
}

std::string Model::HandleTextureName(const char *filename)
{
	std::string name(filename);
	int index = name.find_last_of('\\');
	if(index != -1)
	{
		name.erase(name.begin(), name.begin() + index + 1);
	}
	return name;
}
