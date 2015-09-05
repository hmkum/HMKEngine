#include "Ray.h"

using namespace hmk;

Ray::Ray()
{
	mOrigin = glm::vec3(0.0f);
	mDirection = glm::vec3(0.0f);
}

Ray::Ray(const glm::vec3 &origin, const glm::vec3 &direction)
{
	mOrigin = origin;
	mDirection = direction;
}
bool Ray::IntersectAABB(const BoundingBox &box)
{
	glm::vec3 dirFrac = 1.0f / mDirection;

	float t1 = (box.mMin.x - mOrigin.x) * dirFrac.x;
	float t2 = (box.mMax.x - mOrigin.x) * dirFrac.x;
	float t3 = (box.mMin.y - mOrigin.y) * dirFrac.y;
	float t4 = (box.mMax.y - mOrigin.y) * dirFrac.y;
	float t5 = (box.mMin.z - mOrigin.z) * dirFrac.z;
	float t6 = (box.mMax.z - mOrigin.z) * dirFrac.z;

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