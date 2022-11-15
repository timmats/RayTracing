#include "PhysicsWorld.h"

void PhysicsWorld::simulate(Scene& scene, const float dt)
{
	m_Scene = &scene;

	//for (Sphere sphere : m_Scene->Spheres)
	for (size_t i = 0; i < m_Scene->Spheres.size(); i++)
	{
		Sphere* sphere = &scene.Spheres[i];
		if (sphere->Dynamic)
		{
			sphere->Force += sphere->Mass * m_gravity;

			sphere->Velocity += sphere->Force / sphere->Mass * dt;
			sphere->Position += sphere->Velocity * dt;

			sphere->Force = glm::vec3(0);
		}
	}
	return;
}
