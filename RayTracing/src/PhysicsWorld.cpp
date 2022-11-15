#include "PhysicsWorld.h"

void PhysicsWorld::simulate_Gravity(Scene& scene, const float dt)
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

void PhysicsWorld::DequanLi(Scene& scene, const float dt)
{
	m_Scene = &scene;

	float a = 40.0f;
	float c = 1.833f;
	float d = 0.16f;
	float e = 0.65f;
	float f = 20.0f;
	float k = 55.0f;


	for (size_t i = 0; i < m_Scene->Spheres.size(); i++)
	{
		Sphere* sphere = &scene.Spheres[i];
		if (sphere->Dynamic)
		{
			//sphere->Force += sphere->Mass * m_gravity;

			float x = sphere->Position.x;
			float y = sphere->Position.y;
			float z = sphere->Position.z;

			float dx = a * (y - x) + d * x * z;
			float dy = k * x + f * y - x * z;
			float dz = c * z + x * y - e * x * x;

			sphere->Position.x += dt * dx;
			sphere->Position.y += dt * dy;
			sphere->Position.z += dt * dz;

		}
	}
	return;
}
