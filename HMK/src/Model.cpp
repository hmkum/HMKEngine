#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include "Model.h"
#include "Material.h"
#include "Utility.h"

using namespace hmk;

Model::Model()
	: draw_bounding_box_{false}
	, position_vec_{0}
	, rotation_vec_{0}
	, scale_vec_{0}
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
		HMK_LOG_ERROR("Could not load model: ", modelName, ". Error: ", importer.GetErrorString());
		return false;
	}
	filename_ = modelName;
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

void Model::set_position(glm::vec3 pos)
{
	//glm::vec3 diff = position_vec_ - pos;
	position_vec_ = pos;
	/*for(unsigned int i = 0; i < children_.size(); ++i)
	{
		std::shared_ptr<Model>& child = std::dynamic_pointer_cast<Model>(children_[i]);
		child->set_position(child->get_position() - diff);
	}*/
	update_model_matrix();
}

void Model::offset_position(glm::vec3 offset)
{
	// TODO_HMK: Child offset position test et
	position_vec_ += offset;
	//glm::vec3 diff = position_vec_ - offset;
	/*for(unsigned int i = 0; i < children_.size(); ++i)
	{
		std::shared_ptr<Model>& child = std::dynamic_pointer_cast<Model>(children_[i]);
		child->offset_position(-diff);
	}*/
	update_model_matrix();
}

void Model::set_rotation(float _x, float _y, float _z)
{
	set_rotation(glm::vec3(_x, _y, _z));
}

void Model::set_rotation(glm::vec3 rot)
{
	rotation_vec_ = rot;
	/*for(unsigned int i = 0; i < children_.size(); ++i)
	{
		std::shared_ptr<Model>& child = std::dynamic_pointer_cast<Model>(children_[i]);
		glm::vec3 diff_pos = position_vec_ - child->get_position();
		child->offset_position(-diff_pos);
		diff_pos = glm::rotateY(diff_pos, glm::radians(rot.y));
		child->offset_position(diff_pos);
	}*/
	update_model_matrix();
}

void Model::offset_rotation(float _x, float _y, float _z)
{
	offset_rotation(glm::vec3(_x, _y, _z));
}

void Model::offset_rotation(glm::vec3 rot)
{
	//glm::vec3 diff = rotation_vec_ - rot;
	rotation_vec_ += rot;
	/*for(unsigned int i = 0; i < children_.size(); ++i)
	{
		std::shared_ptr<Model>& child = std::dynamic_pointer_cast<Model>(children_[i]);
		child->set_position(position_vec_);
		child->offset_rotation(rot);
		child->set_position(-position_vec_);
	}*/
	update_model_matrix();
}

void Model::set_scale(glm::vec3 scale)
{
	//glm::vec3 diff = scale_vec_ - scale;
	scale_vec_ = scale;
	/*for(unsigned int i = 0; i < children_.size(); ++i)
	{
		std::shared_ptr<Model>& child = std::dynamic_pointer_cast<Model>(children_[i]);
		child->set_scale(child->get_scale() - diff);
	}*/
	update_model_matrix();
}

void Model::offset_scale(glm::vec3 offset)
{
	// TODO_HMK: Child offset scale test et
	//glm::vec3 diff = scale_vec_ - offset;
	scale_vec_ += offset;
	/*for(unsigned int i = 0; i < children_.size(); ++i)
	{
		std::shared_ptr<Model>& child = std::dynamic_pointer_cast<Model>(children_[i]);
		child->offset_scale(-diff);
	}*/
	update_model_matrix();
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

void Model::set_metallic(float m)
{
	if (m >= 1.0f) m = 1.0f;
	if (m <= 0.0f) m = 0.0f;
	for (auto &mesh : meshes_)
	{
		mesh->set_metallic(m);
	}
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

void Model::update_model_matrix()
{
	glm::mat4 t = glm::translate(position_vec_);
	float x = glm::radians(rotation_vec_.x);
	float y = glm::radians(rotation_vec_.y);
	float z = glm::radians(rotation_vec_.z);

	glm::mat4 xRot = glm::rotate(x, glm::vec3(1, 0, 0));
	glm::mat4 yRot = glm::rotate(y, glm::vec3(0, 1, 0));
	glm::mat4 zRot = glm::rotate(z, glm::vec3(0, 0, 1));
	glm::mat4 r = zRot * yRot * xRot;
	glm::mat4 s = glm::scale(scale_vec_);

	model_matrix_ = t * r * s;
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
