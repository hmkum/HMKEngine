#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include "AlignedAllocation.h"
#include "Entity.h"
#include "Mesh.h"
#include "BoundingBox.h"
#include "ShaderProgram.h"
#include "Utility.h"

namespace hmk
{
/**
	Responsible for rendering models.
*/
class Model : public Entity, public AlignedAllocation<16>
{
public:
	Model();
	~Model();

	bool load(std::string modelName);
	void render();
	void render(ShaderProgram &shader);

	void draw_bounding_box(bool draw);

	inline std::string get_filename() const { return filename_; }

	void set_position(glm::vec3 pos);
	void offset_position(glm::vec3 offset);

	void set_rotation(float _x, float _y, float _z);
	void set_rotation(glm::vec3 rot);
	void offset_rotation(float _x, float _y, float _z);
	void offset_rotation(glm::vec3 rot);

	void set_scale(glm::vec3 scale);
	void offset_scale(glm::vec3 offset);

	void set_roughness(float r);
	void set_metallic(float m);

	inline glm::mat4 get_model_matrix() const { return model_matrix_; }
	inline float get_roughness() const { return meshes_.at(0)->get_roughness(); }
	inline float get_metallic()  const { return meshes_.at(0)->get_metallic(); }
	BoundingBox get_bounding_box() const;
	inline glm::vec3 get_position() const { return position_vec_; }
	inline glm::vec3 get_rotation() const { return rotation_vec_; }
	inline glm::vec3 get_scale() const { return scale_vec_; }

private:
	void update_model_matrix();
	std::string handle_texture_name(const char *filename);
private:
	std::string filename_;
	// Holds meshes of models.
	std::vector<std::shared_ptr<Mesh>> meshes_;
	BoundingBox bounding_box_;
	GLuint bounding_box_vao_id, bounding_box_vbo_id;
	bool draw_bounding_box_;

	glm::mat4 model_matrix_;
	glm::vec3 position_vec_, rotation_vec_, scale_vec_;
};

typedef std::unique_ptr<Model> ModelUPtr;
typedef std::shared_ptr<Model> ModelSPtr;
}