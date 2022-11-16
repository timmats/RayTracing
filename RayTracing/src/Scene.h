#pragma once

#include <vector>
#include <glm/glm.hpp>
//#include "Sphere.cpp"

struct Material
{
	enum Type
	{
		Lmbert = 0, Metal, Dielectric, Emissive
	};
	int type = Type::Lmbert;
	glm::vec3 Albedo{ 1.0f };
	float Roughness = 0.0f;
	float RefractiveIndex = 1.5f;
};

struct Sphere
{
	float Radius = 1.0f;
	glm::vec3 Position{0.0f, 0.0f, 0.0f};
	glm::vec3 Velocity{ 0.0f };
	glm::vec3 Force{ 0.0f };
	float Mass = 1;

	bool Dynamic = false;
	int MaterialIndex = 0;
};

struct Scene
{
	std::vector<Sphere> Spheres;
	std::vector<Material> Materials;

	void AddSphere(Sphere& sphere) { Spheres.push_back(sphere); }
	//void RemoveSphere(Sphere& sphere) 
	//{
	//	//if (!sphere) return;
	//	auto itr = std::find(Spheres.begin(), Spheres.end(), sphere);
	//	if (itr == Spheres.end()) return;
	//	Spheres.erase(itr);
	//}
};