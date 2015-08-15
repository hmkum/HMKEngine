#pragma once
#include <glm/glm.hpp>
#include <string>

namespace hmk
{
struct Material
{
	Material()
		: mName("DefaultMat")
		, mAmbient(glm::vec3(1.0f))
		, mBaseColor(glm::vec3(1.0f))
		, mSpecularColor(glm::vec3(1.0f))
		, mIor(0.0f)
		, mRoughness(0.04f)
		, mMetallic(0.0f)
		, HasTextures(glm::tvec4<int>(0))
	{}
	std::string mName;

	glm::vec3 mAmbient;
	glm::vec3 mBaseColor;
	glm::vec3 mSpecularColor;
	float mIor;      // index of refraction
	float mRoughness;
	float mMetallic;

	std::string mAlbedoTexName;
	std::string mNormalTexName;
	std::string mRoughnessTexName;
	std::string mMetallicTexName;	

	glm::tvec4<int> HasTextures; // x: HasAlbedo?, y: HasNormal, z: HasRoughness, w: HasMetallic;
};
}