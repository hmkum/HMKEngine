#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <glm/common.hpp>
#include "Model.h"
#include "Material.h"
#include "Utility.h"

using namespace hmk;

Model::Model()
	: draw_bounding_box_{false}
	, translation_{1.0f}
	, rotation_{1.0f}
	, scale_{1.0f}
{ }

Model::~Model()
{
}

bool Model::load(std::string modelName)
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

#undef min // min ve max windows kütüphanesi için macro olarak tanýmlanmýþ. Bu yüzden glm'nin fonksiyonlarýný görmüyordu.
		   // Bu yüzden undef yapmak zorunda kaldým.
            bounding_box_.min_corner_.x = glm::min(bounding_box_.min_corner_.x, pos.x);
            bounding_box_.min_corner_.y = glm::min(bounding_box_.min_corner_.y, pos.y);
            bounding_box_.min_corner_.z = glm::min(bounding_box_.min_corner_.z, pos.z);
#undef max
            bounding_box_.max_corner_.x = glm::max(bounding_box_.max_corner_.x, pos.x);
            bounding_box_.max_corner_.y = glm::max(bounding_box_.max_corner_.y, pos.y);
            bounding_box_.max_corner_.z = glm::max(bounding_box_.max_corner_.z, pos.z);

			aiVector3D one3D(1.0f, 1.0f, 1.0f);
			aiVector3D zero3D(0.0f, 0.0f, 0.0f);

			aiVector3D norm = mesh->HasNormals() ? mesh->mNormals[j] : one3D;
			vertices[j].Normal = glm::vec3(norm.x, norm.y, norm.z);

			aiVector3D texCoord = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][j] : zero3D;
			vertices[j].TexCoord0 = glm::vec2(texCoord.x, texCoord.y);

			aiVector3D tangent = mesh->HasTangentsAndBitangents() ? mesh->mTangents[j] : zero3D;
			vertices[j].Tangent = glm::vec3(tangent.x, tangent.y, tangent.z);
		}

		glGenVertexArrays(1, &bounding_box_vao_id);
		glBindVertexArray(bounding_box_vao_id);

		GLfloat bbVertices[] = {
			bounding_box_.max_corner_.x, bounding_box_.max_corner_.y, bounding_box_.min_corner_.z,
			bounding_box_.min_corner_.x, bounding_box_.max_corner_.y, bounding_box_.min_corner_.z,
			bounding_box_.min_corner_.x, bounding_box_.min_corner_.y, bounding_box_.min_corner_.z,
			bounding_box_.max_corner_.x, bounding_box_.min_corner_.y, bounding_box_.min_corner_.z,
			bounding_box_.max_corner_.x, bounding_box_.min_corner_.y, bounding_box_.max_corner_.z,
			bounding_box_.max_corner_.x, bounding_box_.max_corner_.y, bounding_box_.max_corner_.z,
			bounding_box_.min_corner_.x, bounding_box_.max_corner_.y, bounding_box_.max_corner_.z,
			bounding_box_.min_corner_.x, bounding_box_.min_corner_.y, bounding_box_.max_corner_.z,
			bounding_box_.max_corner_.x, bounding_box_.max_corner_.y, bounding_box_.min_corner_.z,
			bounding_box_.max_corner_.x, bounding_box_.max_corner_.y, bounding_box_.max_corner_.z,
			bounding_box_.min_corner_.x, bounding_box_.max_corner_.y, bounding_box_.max_corner_.z,
			bounding_box_.min_corner_.x, bounding_box_.max_corner_.y, bounding_box_.min_corner_.z,
			bounding_box_.max_corner_.x, bounding_box_.min_corner_.y, bounding_box_.max_corner_.z,
			bounding_box_.min_corner_.x, bounding_box_.min_corner_.y, bounding_box_.max_corner_.z,
			bounding_box_.min_corner_.x, bounding_box_.min_corner_.y, bounding_box_.min_corner_.z,
			bounding_box_.max_corner_.x, bounding_box_.min_corner_.y, bounding_box_.min_corner_.z
		};

		glGenBuffers(1, &bounding_box_vbo_id);
		glBindBuffer(GL_ARRAY_BUFFER, bounding_box_vbo_id);
		glBufferData(GL_ARRAY_BUFFER, sizeof(bbVertices), bbVertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindVertexArray(0);

		// Handle material
		Material mat;
		aiColor4D color;
		if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == aiReturn_SUCCESS)
		{
			mat.base_color_ = glm::vec3(color.r, color.g, color.b);
		}

		if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			aiString filename;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &filename);
			mat.albedo_texture_name_ = handle_texture_name(filename.C_Str());
			mat.has_textures_.x = 1;
		}

		if (material->GetTextureCount(aiTextureType_HEIGHT) > 0) // Normal map
		{
			aiString filename;
			material->GetTexture(aiTextureType_HEIGHT, 0, &filename);
			mat.normal_texture_name_ = handle_texture_name(filename.C_Str());
			mat.has_textures_.y = 1;
		}

		if (material->GetTextureCount(aiTextureType_SHININESS) > 0)
		{
			aiString filename;
			material->GetTexture(aiTextureType_SHININESS, 0, &filename);
			mat.roughness_texture_name_ = handle_texture_name(filename.C_Str());
			mat.has_textures_.z = 1;
		}

		if (material->GetTextureCount(aiTextureType_SPECULAR) > 0)
		{
			aiString filename;
			material->GetTexture(aiTextureType_SPECULAR, 0, &filename);
			mat.metallic_texture_name_ = handle_texture_name(filename.C_Str());
			mat.has_textures_.w = 1;
		}

		// Initialize mesh
		std::shared_ptr<Mesh> mesh1 = std::make_shared<Mesh>(vertices, mat, indices);
		meshes_.push_back(mesh1);
	}

	importer.FreeScene();
	return true;
}

void Model::render()
{
	for (auto &mesh : meshes_)
	{
		mesh->render();
	}
}

void Model::render(ShaderProgram &shader)
{
	if(draw_bounding_box_)
	{
		glBindVertexArray(bounding_box_vao_id);
		glDrawArrays(GL_LINE_LOOP, 0, 16 * 3);
		glBindVertexArray(0);
	}

	for(auto &mesh : meshes_)
	{
		mesh->render(shader);
	}
}

glm::mat4 Model::get_model_matrix() const
{
	return scale_ * rotation_ * translation_;
}

void Model::translate(glm::vec3 t)
{
	translation_ = glm::translate(translation_, t);
}

void Model::rotate(float degree, glm::vec3 axis)
{
	rotation_ = glm::rotate(rotation_, glm::radians(degree), axis);
}

void Model::scale(glm::vec3 s)
{
	scale_ = glm::scale(scale_, s);
}

void Model::set_roughness(float r)
{
	if (r >= 1.0f) r = 1.0f;
	if (r <= 0.0f) r = 0.0f;
	for (auto &mesh : meshes_)
	{
		mesh->set_roughness(r);
	}
}

float Model::get_roughness()
{
	return meshes_[0]->get_roughness();
}

void Model::set_metallic(float m)
{
	if (m >= 1.0f) m = 1.0f;
	if (m <= 0.0f) m = 0.0f;
	for (auto &mesh : meshes_)
	{
		mesh->set_metallic(m);
	}
}

float Model::get_metallic()
{
	return meshes_[0]->get_metallic();
}

void Model::draw_bounding_box(bool draw)
{
	draw_bounding_box_ = draw;
}

BoundingBox Model::get_bounding_box() const
{
	BoundingBox box;
	box.min_corner_ = glm::vec3(get_model_matrix() * glm::vec4(bounding_box_.min_corner_, 1.0f));
	box.max_corner_ = glm::vec3(get_model_matrix() * glm::vec4(bounding_box_.max_corner_, 1.0f));
	return box;
}

std::string Model::handle_texture_name(const char *filename)
{
	std::string name(filename);
	int index = name.find_last_of('\\');
	if(index != -1)
	{
		name.erase(name.begin(), name.begin() + index + 1);
	}
	return name;
}
