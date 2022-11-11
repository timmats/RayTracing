#pragma once

#include "Scene.h"
#include <glm/glm.hpp>

class PhysicsWorld
{
public:
	PhysicsWorld() = default;
	PhysicsWorld(const Scene& scene);

	void AddSphere(Sphere* sphere);
	void RemoveSphere(Sphere* sphere);
	void PopSphere();

	void Step(float dt);

private:
	std::vector<Sphere*> m_spheres;
	glm::vec3 m_gravity = glm::vec3(0, -9.81f, 0);
};

