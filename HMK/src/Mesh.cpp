#include "Mesh.h"

using namespace hmk;

Mesh::Mesh(std::vector<Vertex> vertices, Material &material, std::vector<unsigned int> indices)
	: vao_id_{0}
	, vbo_id_{0}
	, ibo_id_{0}
	, material_{material}
	, albedo_texture_{nullptr}
	, normal_texture_{nullptr}
	, roughness_texture_{nullptr}
	, metalness_texture_{nullptr}
{
	indices_size_ = indices.size();

	glGenVertexArrays(1, &vao_id_);
	glBindVertexArray(vao_id_);

	glGenBuffers(1, &vbo_id_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_id_);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices.front(), GL_STATIC_DRAW);
	
	glGenBuffers(1, &ibo_id_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_id_);
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

	if(material.has_textures_.x)
	{
		albedo_texture_ = std::make_shared<Texture>();
		albedo_texture_->create(material.albedo_texture_name_);
	}

	if (material.has_textures_.y)
	{
		normal_texture_ = std::make_shared<Texture>();
		normal_texture_->create(material.normal_texture_name_);
	}

	if (material.has_textures_.z)
	{
		roughness_texture_ = std::make_shared<Texture>();
		roughness_texture_->create(material.roughness_texture_name_);
	}

	if(material.has_textures_.w)
	{
		metalness_texture_ = std::make_shared<Texture>();
		metalness_texture_->create(material.metallic_texture_name_);
	}

	material_uniform_.base_color_ = glm::vec4(material_.base_color_, 1.0f);
}

Mesh::~Mesh()
{
	glDeleteBuffers(1, &vbo_id_);
	glDeleteBuffers(1, &ibo_id_);
	glDeleteVertexArrays(1, &vao_id_);
}

void Mesh::render()
{
	glBindVertexArray(vao_id_);
	glDrawElements(GL_TRIANGLES, indices_size_, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::render(ShaderProgram &shader)
{
	glBindVertexArray(vao_id_);
	
	if (albedo_texture_) albedo_texture_->bind();
	if (normal_texture_) normal_texture_->bind(1);
	if (roughness_texture_) roughness_texture_->bind(2);
	if (metalness_texture_) metalness_texture_->bind(3);
	
	shader.set_uniform("uHasTextures", material_.has_textures_);
	shader.set_uniform("uMaterial", material_uniform_);
	glDrawElements(GL_TRIANGLES, indices_size_, GL_UNSIGNED_INT, 0);

	if (albedo_texture_)	albedo_texture_->unbind();
	if (normal_texture_) normal_texture_->unbind();
	if (roughness_texture_)	roughness_texture_->unbind();
	if (metalness_texture_) metalness_texture_->unbind();

	glBindVertexArray(0);
}

void Mesh::set_roughness(float r)
{
	material_uniform_.roughness_ = r;
}

void Mesh::set_metallic(float m)
{
	material_uniform_.metallic_ = m;
}
