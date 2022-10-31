#pragma once

#include <vector>
#include <glm/glm.hpp>

struct Material
{
	glm::vec3 Albedo{ 1.0f };
	float Roughness = 0.01f;
	float Metallic = 1.0f;
};

struct Sphere
{
	float Radius = 0.5f;
	glm::vec3 Position{0.0f, 0.0f, 0.0f};

	int MaterialIndex = 0;
};

struct Scene
{
	std::vector<Sphere> Spheres;
	std::vector<Material> Materials;
};