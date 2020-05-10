#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/constants.hpp>
#include "Sphere.h"
#include "Box.h"

class Ray
{
public:

	glm::vec3 origin;
	glm::vec3 direction;

	Ray()
	{
		origin = glm::vec3(0.0f, 0.0f, 0.0f);
		direction = glm::vec3(0.0f, 0.0f, 1.0f);
	}

	~Ray() = default;

	bool intersect(const Sphere& sphere, glm::vec3* pos = nullptr, glm::vec3* normal = nullptr)
	{
		float t0, t1; // solutions for t if the ray intersects 

		glm::vec3 L = sphere.pos - origin;
		float tca = glm::dot(L, direction);

		float d2 = glm::dot(L, L) - tca * tca;
		float radius2 = sphere.radius * sphere.radius;
		if (d2 > radius2) return false;
		float thc = sqrtf(radius2 - d2);
		t0 = tca - thc;
		t1 = tca + thc;

		if (t0 > t1) std::swap(t0, t1);

		if (t0 < 0) {
			t0 = t1; // if t0 is negative, use t1 instead 
			if (t0 < 0) return false; // both t0 and t1 are negative 
		}

		if (pos != nullptr)
			*pos = origin + t0 * direction;
		if (normal != nullptr)
			*normal = glm::normalize(*pos - sphere.pos);

		return true;
	}

	bool intersect(const Box& box, const glm::mat4& mv, const glm::mat4& invMV, glm::vec3* pos = nullptr, glm::vec3* normal = nullptr)
	{
		glm::vec4 orig(origin.x, origin.y, origin.z, 1.0f);
		glm::vec4 dir(direction.x, direction.y, direction.z, 0.0f);

		orig = box.invRotMat * invMV * orig;
		dir = glm::normalize(box.invRotMat * invMV * dir);

		glm::vec4 normX(-1, 0, 0, 0);

		float tmin = (box.minPos.x - orig.x) / dir.x;
		float tmax = (box.maxPos.x - orig.x) / dir.x;

		if (tmin > tmax)
		{
			normX = -normX;
			swap(tmin, tmax);
		}
		glm::vec4 norm = normX;

		glm::vec4 normY(0, -1, 0, 0);

		float tymin = (box.minPos.y - orig.y) / dir.y;
		float tymax = (box.maxPos.y - orig.y) / dir.y;

		if (tymin > tymax)
		{
			normY = -normY;
			swap(tymin, tymax);
		}

		if ((tmin > tymax) || (tymin > tmax))
			return false;

		if (tymin > tmin)
		{
			norm = normY;
			tmin = tymin;
		}

		if (tymax < tmax)
			tmax = tymax;

		glm::vec4 normZ(0, 0, -1, 0);

		float tzmin = (box.minPos.z - orig.z) / dir.z;
		float tzmax = (box.maxPos.z - orig.z) / dir.z;

		if (tzmin > tzmax)
		{
			normZ = -normZ;
			swap(tzmin, tzmax);
		}

		if ((tmin > tzmax) || (tzmin > tmax))
			return false;

		if (tzmin > tmin)
		{
			norm = normZ;
			tmin = tzmin;
		}

		if (tzmax < tmax)
			tmax = tzmax;

		if (tmin < 0) return false;

		glm::vec4 intersection = orig + tmin * dir;
		if (pos != nullptr)
			*pos = (mv * box.rotMat * intersection).xyz();
		if (normal != nullptr)
			*normal = glm::normalize(mv * box.rotMat * norm).xyz();
		//*pos = intersection.xyz();

		return true;
	}
};
