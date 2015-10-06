#include "Ray.h"

using namespace hmk;

Ray::Ray()
	: origin_point_{0.0f}
	, direction_{0.0f}
{
}

Ray::Ray(const glm::vec3 &origin, const glm::vec3 &direction)
	: origin_point_{origin}
	, direction_{direction}
{
}

bool Ray::intersect_aabb(const BoundingBox &box)
{
	glm::vec3 dirFrac = 1.0f / direction_;

	float t1 = (box.min_corner_.x - origin_point_.x) * dirFrac.x;
	float t2 = (box.max_corner_.x - origin_point_.x) * dirFrac.x;
	float t3 = (box.min_corner_.y - origin_point_.y) * dirFrac.y;
	float t4 = (box.max_corner_.y - origin_point_.y) * dirFrac.y;
	float t5 = (box.min_corner_.z - origin_point_.z) * dirFrac.z;
	float t6 = (box.max_corner_.z - origin_point_.z) * dirFrac.z;

	float tmin = glm::max(glm::max(glm::min(t1, t2), glm::min(t3, t4)), glm::min(t5, t6));
	float tmax = glm::min(glm::min(glm::max(t1, t2), glm::max(t3, t4)), glm::max(t5, t6));

	// if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behing us
	if (tmax < 0)
	{
		return false;
	}
	// if tmin > tmax, ray doesn't intersect AABB
	if (tmin > tmax)
	{
		return false;
	}
	
	return true;
}