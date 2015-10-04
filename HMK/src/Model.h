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

	void DrawBoundingBox(bool draw);
	BoundingBox GetBoundingBox() const;

private:
	std::string HandleTextureName(const char *filename);
private:
	// Holds meshes of models.
	std::vector<std::shared_ptr<Mesh>> mMeshes;
	BoundingBox mBoundingBox;
	GLuint mBBVAO, mBBVBO;
	bool mDrawBoundingBox;

	glm::mat4 mTranslation;
	glm::mat4 mRotation;
	glm::mat4 mScale;
};

typedef std::shared_ptr<Model> ModelPtr;
}