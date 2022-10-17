#include "Renderer.h"
#include "Walnut/Random.h"


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


void Renderer::Render()
{
	// Height first is CPU cache friendly. 
	for (uint32_t j = 0; j < m_FinalImage->GetHeight(); j++)
	{
		for (uint32_t i = 0; i < m_FinalImage->GetWidth(); i++)
		{
			// uv coord on screen
			glm::vec2 coord = { (float)i / (float)m_FinalImage->GetWidth(), (float)j / (float)m_FinalImage->GetHeight() };
			coord = coord * 2.0f - 1.0f; // scales to (-1, 1)
			// fills horizontally first, from bottom to top
			m_ImageData[i + j * m_FinalImage->GetWidth()] = PerPixel(coord);
		}
	}

	m_FinalImage->SetData(m_ImageData);
}

uint32_t Renderer::PerPixel(glm::vec2 coord)
{
	uint8_t r = (uint8_t)(coord.x * 255.0f);
	uint8_t g = (uint8_t)(coord.y * 255.0f);

	// ||b||^2 t^2 + 2 <a, b> t + (||a||^2- r^2) = 0
	// a = ray origin 
	// b = ray direction
	// r = radius
	// t = hit distance

	// coord is the ray dir
	glm::vec3 rayOrigin(0.0f, 0.0f, 2.0f);
	glm::vec3 rayDirection(coord.x, coord.y, -1.0f);
	float radius = 0.5f;

	rayDirection = glm::normalize(rayDirection);

	float a = glm::dot(rayDirection, rayDirection);
	float b = 2 * glm::dot(rayOrigin, rayDirection);
	float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;

	// quadratic formula discriminant: b^2 -4ac
	float discriminant = b * b - 4 * a * c;

	if (discriminant > 0.0f)/*
		return 0xff000000 | (g << 8) | r;*/
		return 0xffff00ff;
	return 0xff000000;
}