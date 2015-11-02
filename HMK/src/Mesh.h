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
	void render(ShaderProgramSPtr shader);

	void set_roughness(float r);
	void set_metallic(float m);

	inline float get_roughness() const { return material_uniform_.roughness_; }
	inline float get_metallic() const { return material_uniform_.metallic_; }

private:
	GLuint vao_id_, vbo_id_, ibo_id_;
	Material material_;
	MaterialUniform material_uniform_;
	unsigned int indices_size_;
	std::vector<std::shared_ptr<Texture>> textures_;
};
}