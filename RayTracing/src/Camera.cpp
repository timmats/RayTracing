#include "Camera.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Walnut/Input/Input.h"

using namespace Walnut;

static const float SENSITIVITY = 0.002f;
constexpr glm::vec3 UpDirection = glm::vec3(0.0f, 1.0f, 0.0f);

Camera::Camera(float verticalFOV, float nearClip, float farClip)
	: m_VerticalFOV(verticalFOV), m_NearClip(nearClip), m_FarClip(farClip)
{
	m_Position = glm::vec3(0, 0, 3);
	m_ForwardDirection = glm::vec3(0, 0, -1);
}

void Camera::OnUpdate(float ts)
{
	if  (!Input::IsMouseButtonDown(MouseButton::Right))
	{
		Input::SetCursorMode(CursorMode::Normal);
		return;
	}

	glm::vec2 mousePos = Input::GetMousePosition();
	glm::vec2 delta = (mousePos - m_LastMousePosition) * SENSITIVITY; // corresponds to theta and phi angles
	m_LastMousePosition = mousePos;

	bool moved = false;
	const float speed = 5.0f;

	
	glm::vec3 RightDirection = glm::cross(m_ForwardDirection, UpDirection);

	// Move camera
	if (Input::IsKeyDown(KeyCode::W))
	{
		m_Position += m_ForwardDirection * speed * ts;
		moved = true;
	}
	else if (Input::IsKeyDown(KeyCode::S))
	{
		m_Position -= m_ForwardDirection * speed * ts;
		moved = true;
	}
	else if(Input::IsKeyDown(KeyCode::A))
	{
		m_Position -= RightDirection * speed * ts;
		moved = true;
	}
	else if(Input::IsKeyDown(KeyCode::D))
	{
		m_Position += RightDirection * speed * ts;
		moved = true;
	}
	else if(Input::IsKeyDown(KeyCode::Q))
	{
		m_Position += UpDirection * speed * ts;
		moved = true;
	}
	else if(Input::IsKeyDown(KeyCode::E))
	{
		m_Position -= UpDirection * speed * ts;
		moved = true;
	}

	//Rotation 
	if (delta.x != 0.0f || delta.y != 0.0f)
	{
		float theta = delta.x * GetRotationSpeed(); // yawDelta
		float phi = delta.y * GetRotationSpeed();	// pitchDelta

		glm::quat q = glm::normalize(glm::cross(glm::angleAxis(-phi, RightDirection), glm::angleAxis(-theta, UpDirection)));
		m_ForwardDirection = glm::rotate(q, m_ForwardDirection);

		moved = true;
	}

	if (moved)
	{
		ReCalculateView();
		ReCalculateRayDirections();
	}
}

void Camera::OnResize(uint32_t width, uint32_t height)
{
	if (width != m_ViewportWidth || height != m_ViewportHeight) return;

	m_ViewportWidth = width;
	m_ViewportHeight = height;

	ReCalculateProjection();
	ReCalculateRayDirections();
}

float Camera::GetRotationSpeed() { return 0.3f; }

void Camera::ReCalculateView()
{
	m_View = glm::lookAt(m_Position, m_Position + m_ForwardDirection, UpDirection);
	m_InverseView = glm::inverse(m_View);
}

void Camera::ReCalculateProjection()
{
	m_Projection = glm::perspectiveFov(m_VerticalFOV, (float)m_ViewportWidth, (float)m_ViewportHeight, m_NearClip, m_FarClip);
	m_InverseView = glm::inverse(m_Projection);
}

void Camera::ReCalculateRayDirections()
{
	m_RayDirections.resize(m_ViewportWidth * m_ViewportHeight);

	for (uint32_t j = 0; j < m_ViewportHeight; j++)
	{
		for (uint32_t i = 0; i < m_ViewportWidth; i++)
		{
			// uv coord on screen
			glm::vec2 coord = { (float)i / (float)m_ViewportWidth, (float)j / (float)m_ViewportHeight };
			coord = coord * 2.0f - 1.0f; // scales to (-1, 1)

			glm::vec4 target = m_InverseProjection * glm::vec4(coord.x, coord.y, 1.0f, 1.0f);
			glm::vec3 rayDirection = glm::vec3(m_InverseView * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0));
			m_RayDirections[i + j * m_ViewportWidth] = rayDirection;
		}
	}
}