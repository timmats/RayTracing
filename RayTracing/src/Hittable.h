#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "Ray.h"

struct Hit_Status
{
	bool isHit = false;
	float t;
};

class Hittable {
public:
	virtual void Hit(Ray& ray, Hit_Status& status) const = 0;
};