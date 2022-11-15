#pragma once

#include "Scene.h"

class PhysicsWorld
{
public:
	PhysicsWorld() = default;

	void simulate_Gravity(Scene& scene, const float dt);
	void DequanLi(Scene& scene, const float dt);

private:
	Scene* m_Scene = nullptr;
	const glm::vec3 m_gravity = { 0,  -9.81f, 0 };
};

