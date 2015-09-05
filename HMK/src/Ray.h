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
	bool IntersectAABB(const BoundingBox &box);

private:
	glm::vec3 mOrigin;
	glm::vec3 mDirection;
};
}