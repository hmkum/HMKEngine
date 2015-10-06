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

	void render();
	void render(ShaderProgram &shader);

	void set_roughness(float r);
	float get_roughness();

	void set_metallic(float m);
	float get_metallic();

private:
	GLuint vao_id_, vbo_id_, ibo_id_;
	Material material_;
	MaterialUniform material_uniform_;
	unsigned int indices_size_;
	std::shared_ptr<Texture> albedo_texture_, normal_texture_, roughness_texture_, metalness_texture_;
};
}