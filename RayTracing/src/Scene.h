#pragma once

#include <vector>
#include <glm/glm.hpp>
//#include "Sphere.cpp"

struct Material
{
	enum Type
	{
		Lmbert = 0, Metal, Dielectric
	};
	int type = Type::Dielectric;
	glm::vec3 Albedo{ 1.0f };
	float Roughness = 0.01f;
	float Metallic = 0.5f;
	float RefractiveIndex = 1.5f;
};

struct Sphere
{
	float Radius = 0.5f;
	glm::vec3 Position{0.0f, 0.0f, 0.0f};
	//glm::vec3 Velocity{ 0.0f };
	//glm::vec3 Force{ 0.0f };
	//float Mass = 1;

	int MaterialIndex = 0;
};

struct Scene
{
	
	std::vector<Sphere> Spheres;
	std::vector<Material> Materials;
};