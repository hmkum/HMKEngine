#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include "Mesh.h"
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
	bool Load(std::string modelName);
	void Render();
	void Render(ShaderProgram &shader);

	glm::mat4 GetModelMatrix() const;

	void Translate(glm::vec3 t);
	void Rotate(float degree, glm::vec3 axis);
	void Scale(glm::vec3 s);

	void SetRoughness(float r);
	float GetRoughness();

	void SetMetallic(float m);
	float GetMetallic();

private:
	std::string HandleTextureName(const char *filename);
private:
	// Holds meshes of models.
	std::vector<std::shared_ptr<Mesh>> mMeshes;

	glm::mat4 mTranslation;
	glm::mat4 mRotation;
	glm::mat4 mScale;
};
}