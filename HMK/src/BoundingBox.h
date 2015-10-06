#pragma once
#include <glm/glm.hpp>

namespace hmk
{
class BoundingBox
{
public:
	BoundingBox()
		: min_corner_{glm::vec3(999999.0f)}
		, max_corner_{glm::vec3(-999999.0f)}
	{}

	glm::vec3 min_corner_, max_corner_;
};
}