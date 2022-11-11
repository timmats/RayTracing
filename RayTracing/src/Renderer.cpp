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

	delete[] m_AccumulationData;
	m_AccumulationData = new glm::vec4[width * height];

}


void Renderer::Render(const Scene& scene, const Camera& camera)
{

	m_ActiveCamera = &camera;
	m_ActiveScene = &scene;

	if (m_FrameIndex == 1)
		memset(m_AccumulationData, 0, m_FinalImage->GetWidth() * m_FinalImage->GetHeight() * sizeof(glm::vec4));

	// Height first is CPU cache friendly. 
	for (uint32_t j = 0; j < m_FinalImage->GetHeight(); j++)
	{
		for (uint32_t i = 0; i < m_FinalImage->GetWidth(); i++)
		{
			// fills horizontally first, from bottom to top
			glm::vec4 color = PerPixel(i, j);
			m_AccumulationData[i + j * m_FinalImage->GetWidth()] += color;

			glm::vec4 accumulatedColor = m_AccumulationData[i + j * m_FinalImage->GetWidth()];
			accumulatedColor /= (float)m_FrameIndex;

			accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));
			m_ImageData[i + j * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(accumulatedColor);
		}
	}

	m_FinalImage->SetData(m_ImageData);

	if (m_Settings.Accumulate)
		m_FrameIndex++;
	else
		m_FrameIndex = 1;
}



glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y) //RayGen
{
	Ray ray;
	ray.Origin = m_ActiveCamera->GetPosition();
	ray.Direction = m_ActiveCamera->GetRayDirections()[x + y * m_FinalImage->GetWidth()];
	
	glm::vec3 color(0.0f);

	float multiplier = 1.0f;

	int bounces = 4;
	for (int i = 0; i < bounces; i++)
	{
		Renderer::HitPayload payload = TraceRay(ray);

		if (payload.HitDistance < 0.0f)
		{
			// gradient background
			float t = 0.5f * (ray.Direction.y + 1.0);
			glm::vec3 skycolor = (1 - t) * glm::vec3(1.0f) + t * glm::vec3(0.6f, 0.7f, 0.9f);
			color += skycolor * multiplier;
			break;
		}

		// light source origin - sphere origin(= 0 bcz camera position is shifted)
		glm::vec3 lightDirection = -glm::normalize(glm::vec3(1)); 
		float lightIntensity = glm::max(glm::dot(payload.WorldNormal, -lightDirection), 0.0f);

		const Sphere& sphere = m_ActiveScene->Spheres[payload.ObjectIndex];
		const Material& material = m_ActiveScene->Materials[sphere.MaterialIndex];
		color += multiplier * material.Albedo * lightIntensity;

		multiplier *= material.Metallic;

		ray.Origin = payload.WorldPosition + 0.0001f * payload.WorldNormal;
		ray.Direction = glm::reflect(ray.Direction, 
			payload.WorldNormal + material.Roughness * Walnut::Random::InUnitSphere() * Walnut::Random::Float());
	}

	return glm::vec4(color, 1.0f);
}

Renderer::HitPayload  Renderer::TraceRay(const Ray& ray)
{
	// ||v||^2 t^2 + 2 <u, v> t + (||u||^2- r^2) = 0
	// u = ray origin 
	// v = ray direction
	// r = radius
	// t = hit distance

	int closestObjectIndex = -1;
	float hitDistance = FLT_MAX;

	for (size_t i = 0; i < m_ActiveScene->Spheres.size(); i++)
	{
		const Sphere& sphere = m_ActiveScene->Spheres[i];
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


		if (closestT > 0.0f && closestT < hitDistance)
		{
			closestObjectIndex = (int)i;
			hitDistance = closestT;
		}
	}

	if (closestObjectIndex < 0)
		return Miss(ray);

	return ClosestHit(ray, hitDistance, closestObjectIndex);
}


Renderer::HitPayload Renderer::ClosestHit(const Ray& ray, float hitDistacne, int objectIndex)
{
	Renderer::HitPayload payload;
	payload.HitDistance = hitDistacne;
	payload.ObjectIndex = objectIndex;
	
	const Sphere& closestSphere = m_ActiveScene->Spheres[objectIndex];

	glm::vec3 origin = ray.Origin - closestSphere.Position;

	payload.WorldPosition = hitDistacne * ray.Direction + origin;
	payload.WorldNormal = glm::normalize(payload.WorldPosition);
	payload.WorldPosition += closestSphere.Position;

	return payload;
}

Renderer::HitPayload Renderer::Miss(const Ray& ray)
{
	Renderer::HitPayload payload;
	payload.HitDistance = -1.0f;
	return payload;
}
