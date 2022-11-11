#include "PhysicsWorld.h"
#include "Scene.h"

PhysicsWorld::PhysicsWorld(const Scene& scene)
{
	for (Sphere sphere : scene.Spheres)
	{
		m_spheres.push_back(&sphere);
	}
}
void PhysicsWorld::AddSphere(Sphere* sphere)
{
	m_spheres.push_back(sphere);
}

void PhysicsWorld::PopSphere()
{
	m_spheres.pop_back();
}
void PhysicsWorld::RemoveSphere(Sphere* sphere)
{
	std::vector<Sphere*>::iterator new_end;
	new_end = remove(m_spheres.begin(), m_spheres.end(), sphere);
}

void PhysicsWorld::Step(float dt) 
{
	for (Sphere* sphere : m_spheres)
	{
		sphere->Force += sphere->Mass * m_gravity; //apply force

		sphere->Velocity += sphere->Force / sphere->Mass * dt;
		sphere->Position += sphere->Velocity * dt;

		sphere->Force = glm::vec3(0.0f);	// reset net force
	}
}