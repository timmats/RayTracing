#pragma once

#include "Hittable.h"

class ASphere : Hittable
{
public:
	ASphere() {};

	ASphere(glm::vec3 pos, float radius) : Radius(radius) { Position = pos; }

	virtual void Hit(Ray& ray, Hit_Status& status) const override;
public:
	float Radius = 0.5f;
	glm::vec3 Position{ 0.0f, 0.0f, 0.0f };
	int MaterialIndex = 0;
	float t = 0;
	bool isHit = false;
private:
};

void ASphere::Hit(Ray& ray, Hit_Status& status) const{
	glm::vec3 origin = ray.Origin - Position;

	float a = glm::dot(ray.Direction, ray.Direction);
	float b = 2 * glm::dot(origin, ray.Direction);
	float c = glm::dot(origin, origin) - Radius * Radius;

	// quadratic formula discriminant: b^2 -4ac
	float discriminant = b * b - 4 * a * c;

	// ray never hits
	if (discriminant < 0.0f)
		status.isHit = false;

	// if hit, t = (-b +/- sqrt(discriminant)) / 2a
	//float t1 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
	status.t = (-b - glm::sqrt(discriminant)) / (2.0f * a);
	status.isHit = true;

	return;
}
