#include "InputSystem.h"
#include <GLFW/glfw3.h>

void InputSystem::ObserveKey(int key)
{
	m_keyMapper[key] = KeyboardObserver(m_window, key);
}

void InputSystem::Update()
{
	for (auto i = m_keyMapper.begin(); i != m_keyMapper.end(); i++)
		i->second.Update();
}

bool InputSystem::LeftMouseDown()
{
	return (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
}

bool InputSystem::RightMouseDown()
{
	return (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
}

void InputSystem::GetPickingRay(const glm::mat4& transformationMatrix, glm::vec3& startingPoint, glm::vec3& direction)
{
	double xPos, yPos;

	glfwGetCursorPos(m_window, &xPos, &yPos);

	int screenWidth, screenHeight;
	glfwGetFramebufferSize(m_window, &screenWidth, &screenHeight);
	xPos = (xPos / screenWidth) * 2.0 - 1.0;
	yPos = 1.0 - 2.0 * (yPos / screenHeight);

	glm::vec4 nearPoint = glm::vec4((float)xPos, (float)yPos, -0.99f, 1.0f);
	glm::vec4 farPoint = glm::vec4((float)xPos, (float)yPos, 0.99f, 1.0f);

	glm::mat4 inverse = glm::inverse(transformationMatrix);

	nearPoint = inverse * nearPoint;
	farPoint = inverse * farPoint;

	nearPoint /= nearPoint.w;
	farPoint /= farPoint.w;

	startingPoint = nearPoint;
	direction = farPoint - nearPoint;
	direction = glm::normalize(direction);
}

// Returns mouse position from Top-Left(0,0) to Bottom-Right(1,1)
void InputSystem::GetMousePosition(double& xPos, double& yPos)
{
	double pixelPosX, pixelPosY;
	glfwGetCursorPos(m_window, &pixelPosX, &pixelPosY);
	
	int screenWidth, screenHeight;
	glfwGetFramebufferSize(m_window, &screenWidth, &screenHeight);

	xPos = pixelPosX / screenWidth;
	yPos = pixelPosY / screenHeight;
}
