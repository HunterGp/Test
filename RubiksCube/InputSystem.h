#pragma once
#include "KeyboardObserver.h"
#include <map>
#include <glm/mat4x4.hpp>

class InputSystem
{
public:
	InputSystem() { m_window = nullptr; }

	void SetWindow(GLFWwindow* window) { m_window = window; }
	void ObserveKey(int key);
	void Update();

	bool IsDown(int key) { return m_keyMapper[key].m_isDown; }
	bool Pressed(int key) { return m_keyMapper[key].m_wasPressed; }
	bool Released(int key) { return m_keyMapper[key].m_wasReleased; }

	bool LeftMouseDown();
	bool RightMouseDown();

	void GetPickingRay(const glm::mat4& transformationMatrix, glm::vec3& startingPoint, glm::vec3& direction);
	void GetMousePosition(double& xPos, double& yPos);

private:
	GLFWwindow* m_window;
	std::map<int, KeyboardObserver> m_keyMapper;
};

