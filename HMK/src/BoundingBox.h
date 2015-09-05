#pragma once
#include <glm/glm.hpp>

namespace hmk
{
class BoundingBox
{
public:
	BoundingBox()
	{
		mMin = glm::vec3(999999.0f);
		mMax = glm::vec3(-999999.0f);
	}

	glm::vec3 mMin, mMax;
};
}