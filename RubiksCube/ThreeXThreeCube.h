#pragma once
#include <glm/ext/quaternion_float.hpp>
#include "CubieRenderer.h"
#include "GameInterface.h"
#include "InputSystem.h"

const int CUBE_SIZE = 3;
const float MOUSE_SENSITIVITY = 30000.0f;
const float ROTATION_SPEED = 150.0f;
const float TWIST_SPEED = 720.0f;
const glm::vec4 FORWARD_AXIS = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
const glm::vec4 RIGHT_AXIS = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
const glm::vec4 UP_AXIS = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

enum Face
{
	FRONT,
	BACK,
	RIGHT,
	LEFT,
	TOP,
	BOTTOM
};

class ThreeXThreeCube : public GameInterface
{
public:
	void Initialize(GLFWwindow* window) override;
	void Render(float aspectRatio) override;
	void ClearResource() override;
	void Update(double deltaTime) override;

private:
	CubieRenderer m_cubieRenderer;
	InputSystem m_input;
	GLFWwindow* m_window;

	bool m_rotating;
	float m_degreesRotated;
	int m_rotationDirection;
	Face m_rotatingFace;
	std::vector<glm::mat4*> m_cubiesOnCurrentFace;

	glm::mat4 m_transform[CUBE_SIZE][CUBE_SIZE][CUBE_SIZE];
	float m_cubieOffset;

	glm::vec4 m_forwardVector;
	glm::vec4 m_rightVector;
	glm::vec4 m_upVector;

	double m_mousePosX;
	double m_mousePosY;
	double m_mousePosOldX;
	double m_mousePosOldY;
	double m_mouseVelX;
	double m_mouseVelY;

private:
	void PrintMat4(const glm::mat4& mat);
	void PrintVec(const glm::vec4& vec);
	void PrintVec(const glm::vec3& vec);
	void RotateCubie(const glm::vec3 axis, const float degrees, glm::mat4& cubieTransform);
	void RotateRubiksCube(const glm::vec3 euler);
	void RotateFace(Face face, const std::vector<glm::mat4*>& cubiesOnFace, float degree);
	void GetCameraRelativeDirectionVectors(glm::vec3& outForward, glm::vec3& outRight, glm::vec3& outUp);
	std::vector<glm::mat4*> GetCubiesOnFace(Face face);
	void UpdateMouseData();
	bool EqualWithError(glm::vec3 a, glm::vec3 b, float allowedError);
	void PlayTwistSound();
};

