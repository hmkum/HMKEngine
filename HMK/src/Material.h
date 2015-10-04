#pragma once
#include <glm/glm.hpp>
#include <string>

namespace hmk
{

struct MaterialUniform
{
	MaterialUniform()
		: mBaseColor{glm::vec4(0.0f)}
		, mRoughness{0.0f}
		, mMetallic{0.0f}
	{}

	glm::vec4 mBaseColor;
	float mRoughness;
	float mMetallic;
};

struct Material
{
	Material()
		: mName{"DefaultMat"}
		, mBaseColor{glm::vec3(1.0f)}
		, mRoughness{0.04f}
		, mMetallic{0.0f}
		, mHasTextures{glm::ivec4(0)}
	{}
	std::string mName;
	
	glm::vec3 mBaseColor;
	float mRoughness;
	float mMetallic;
	
	std::string mAlbedoTexName;
	std::string mNormalTexName;
	std::string mRoughnessTexName;
	std::string mMetallicTexName;	
	
	glm::ivec4 mHasTextures; // x: HasAlbedo?, y: HasNormal, z: HasRoughness, w: HasMetallic;
};
}