#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include "Mesh.h"
#include "BoundingBox.h"
#include "ShaderProgram.h"
#include "Utility.h"

namespace hmk
{
/**
	Responsible for rendering models.
*/
class Model
{
public:
	Model();
	~Model();

	bool load(std::string modelName);
	void render();
	void render(ShaderProgram &shader);

	glm::mat4 get_model_matrix() const;

	void translate(glm::vec3 t);
	void rotate(float degree, glm::vec3 axis);
	void scale(glm::vec3 s);

	void set_roughness(float r);
	float get_roughness();

	void set_metallic(float m);
	float get_metallic();

	void draw_bounding_box(bool draw);
	BoundingBox get_bounding_box() const;

private:
	std::string handle_texture_name(const char *filename);
private:
	// Holds meshes of models.
	std::vector<std::shared_ptr<Mesh>> meshes_;
	BoundingBox bounding_box_;
	GLuint bounding_box_vao_id, bounding_box_vbo_id;
	bool draw_bounding_box_;

	glm::mat4 translation_;
	glm::mat4 rotation_;
	glm::mat4 scale_;
};

typedef std::shared_ptr<Model> ModelPtr;
}