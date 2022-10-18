#include "Renderer.h"
#include "Walnut/Random.h"
#include "Ray.h"

namespace Utils {
	static uint32_t ConvertToRGBA(const glm::vec4& color)
	{
		// assume color is normalized
		uint8_t r =  (uint8_t)(color.r * 255.0f);
		uint8_t g =  (uint8_t)(color.g * 255.0f);
		uint8_t b =  (uint8_t)(color.b * 255.0f);
		uint8_t a =  (uint8_t)(color.a * 255.0f);

		uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
		return result;
	}
}

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (m_FinalImage)
	{
		// No resize necessary
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
			return;
		m_FinalImage->Resize(width, height);
	}
	else
	{
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}
	
	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];

}


void Renderer::Render(const Scene& scene, const Camera& camera)
{
	Ray ray;
	ray.Origin = camera.GetPosition();
	m_ActiveCamera = &camera;

	// Height first is CPU cache friendly. 
	for (uint32_t j = 0; j < m_FinalImage->GetHeight(); j++)
	{
		for (uint32_t i = 0; i < m_FinalImage->GetWidth(); i++)
		{
			// uv coord on screen
			ray.Direction = camera.GetRayDirections()[i + j * m_FinalImage->GetWidth()];

			// fills horizontally first, from bottom to top
			glm::vec4 color = TraceRay(scene, ray);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
			m_ImageData[i + j * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(color);
		}
	}

	m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::TraceRay(const Scene& scene, const Ray& ray)
{
	// ||v||^2 t^2 + 2 <u, v> t + (||u||^2- r^2) = 0
	// u = ray origin 
	// v = ray direction
	// r = radius
	// t = hit distance

	if (scene.Spheres.size() == 0)
		return glm::vec4(0, 0, 0, 1);

	const Sphere* ClosestSphere = nullptr;
	float HitDistance = FLT_MAX;

	for (const Sphere& sphere : scene.Spheres)
	{

		glm::vec3 origin = ray.Origin - sphere.Position;

		float a = glm::dot(ray.Direction, ray.Direction);
		float b = 2 * glm::dot(origin, ray.Direction);
		float c = glm::dot(origin, origin) - sphere.Radius * sphere.Radius;

		// quadratic formula discriminant: b^2 -4ac
		float discriminant = b * b - 4 * a * c;

		// ray never hits
		if (discriminant < 0.0f) 
			continue;

		// if hit, t = (-b +/- sqrt(discriminant)) / 2a
		//float t1 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
		float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);
		if (closestT < HitDistance)
		{
			ClosestSphere = &sphere;
			HitDistance = closestT;
		}
	}

	if (ClosestSphere == nullptr)
		return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	glm::vec3 origin = ray.Origin - ClosestSphere->Position;

	glm::vec3 hitPoint = HitDistance * ray.Direction + origin;
	glm::vec3 sphereNormal = glm::normalize(hitPoint );

	glm::vec3 LightDirection = -glm::normalize(glm::vec3(1));
	float LightIntensity = glm::max(glm::dot(sphereNormal, -  LightDirection), 0.0f);

	return glm::vec4(ClosestSphere->Albedo * LightIntensity, 1.0f);
}