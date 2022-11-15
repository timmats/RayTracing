#pragma once

#include "Scene.h"

class PhysicsWorld
{
public:
	PhysicsWorld() = default;

	void simulate(Scene& scene, const float dt);

private:
	Scene* m_Scene = nullptr;
	const glm::vec3 m_gravity = { 0,  -9.81f, 0 };
};

