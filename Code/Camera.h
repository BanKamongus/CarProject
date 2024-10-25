#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Application.h"

class Camera
{
public:
	Camera()
		: m_position {0.0f}
		, m_lookAt {0.0f}
	{

	}

	virtual ~Camera() = default;

	virtual glm::mat4 GetViewMatrix() const
	{
		return glm::lookAt(m_position, m_lookAt, glm::vec3(0, 1, 0));
	}

	virtual glm::mat4 GetViewProjectionMatrix() const
	{
		glm::vec2 windowSize = Application::Get().GetWindowSize();
		glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)windowSize.x / (float)windowSize.y, 0.1f, 1000.0f);

		return projection * GetViewMatrix();
	}

	glm::vec3 GetPosition() const
	{
		return m_position;
	}

	void SetPosition(const glm::vec3& position)
	{
		m_position = position;
	}

protected:
	glm::vec3 m_position;
	glm::vec3 m_lookAt;
};