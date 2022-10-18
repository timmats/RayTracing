#include "Renderer.h"
#include "Walnut/Random.h"

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


void Renderer::Render(const Camera& camera)
{
	//const glm::vec3& rayOrigin = camera.GetPosition();
	m_ActiveCamera = &camera;

	// Height first is CPU cache friendly. 
	for (uint32_t j = 0; j < m_FinalImage->GetHeight(); j++)
	{
		for (uint32_t i = 0; i < m_FinalImage->GetWidth(); i++)
		{
			// uv coord on screen
			glm::vec2 coord = { (float)i / (float)m_FinalImage->GetWidth(), (float)j / (float)m_FinalImage->GetHeight() };
			coord = coord * 2.0f - 1.0f; // scales to (-1, 1)
			//const glm::vec3& rayDirection = camera.GetRayDirections()[i + j * m_FinalImage->GetWidth()];

			// fills horizontally first, from bottom to top
			glm::vec4 color = PerPixel(coord);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
			m_ImageData[i + j * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(color);
		}
	}

	m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::PerPixel(glm::vec2 coord)
{
	// ||v||^2 t^2 + 2 <u, v> t + (||u||^2- r^2) = 0
	// u = ray origin 
	// v = ray direction
	// r = radius
	// t = hit distance

	// coord is the ray dir
	//glm::vec3 rayOrigin(0.0f, 0.0f, 1.0f);
	glm::vec3 rayDirection(coord.x, coord.y, -1.0f);
	 
	glm::vec3 rayOrigin = m_ActiveCamera->GetPosition();
	//glm::vec3 rayDirection = m_ActiveCamera->GetRayDirections();


	float radius = 0.5f;

	//rayDirection = glm::normalize(rayDirection);

	float a = glm::dot(rayDirection, rayDirection);
	float b = 2 * glm::dot(rayOrigin, rayDirection);
	float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;

	// quadratic formula discriminant: b^2 -4ac
	float discriminant = b * b - 4 * a * c;


	// ray never hits
	if (discriminant < 0.0f) 
		return glm::vec4(0, 0, 0, 1);

	// if hit, t = (-b +/- sqrt(discriminant)) / 2a
	float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);
	//float t1 = (-b + glm::sqrt(discriminant)) / (2.0f * a);

	glm::vec3 hitPoint = closestT * rayDirection + rayOrigin;
	glm::vec3 sphereNormal = glm::normalize(hitPoint );
	//glm::vec3 h1 = t1 * rayDirection + rayOrigin;

	glm::vec3 LightDirection = -glm::normalize(glm::vec3(1));
	float cos = glm::max(glm::dot(sphereNormal, -  LightDirection), 0.0f);

	glm::vec3 sphereColor(1, 0, 1);
	sphereColor *= cos;
	return glm::vec4(sphereColor, 1.0f);
	
	
}