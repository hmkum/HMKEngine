#pragma once
#include <glm/glm.hpp>
#include "BoundingBox.h"

namespace hmk
{
class Ray
{
public:
	Ray();
	Ray(const glm::vec3 &origin, const glm::vec3 &direction);

	bool intersect_aabb(const BoundingBox &box);

private:
	glm::vec3 origin_point_;
	glm::vec3 direction_;
};
}