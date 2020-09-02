#pragma once

#include "vector.h"

struct Ray {
	Vec3 origin;
	Vec3 direction;
	float length;

	Ray() : origin(Vec3(0, 0, 0)), direction(Vec3(0, 0, 0)), length(10000.f) {}
	Ray(const Vec3& origin, const Vec3& direction, float length) : origin(origin), direction(direction), length(length) {}
};