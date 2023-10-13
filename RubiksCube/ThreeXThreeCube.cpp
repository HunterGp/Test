#include "ThreeXThreeCube.h"
#include <glm/ext.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include <windows.h>
#include "ShaderUtil.h"
#include <random>

void ThreeXThreeCube::Initialize(GLFWwindow* window)
{
	m_cubieRenderer.Initialize();
	m_window = window;

	m_input.SetWindow(window);
	m_input.GetMousePosition(m_mousePosOldX, m_mousePosOldY);
	m_input.ObserveKey(GLFW_KEY_W);
	m_input.ObserveKey(GLFW_KEY_A);
	m_input.ObserveKey(GLFW_KEY_S);
	m_input.ObserveKey(GLFW_KEY_D);
	m_input.ObserveKey(GLFW_KEY_Q);
	m_input.ObserveKey(GLFW_KEY_E);
	m_input.ObserveKey(GLFW_KEY_I);
	m_input.ObserveKey(GLFW_KEY_J);
	m_input.ObserveKey(GLFW_KEY_K);
	m_input.ObserveKey(GLFW_KEY_L);
	m_input.ObserveKey(GLFW_KEY_COMMA);
	m_input.ObserveKey(GLFW_KEY_KP_2);
	m_input.ObserveKey(GLFW_KEY_KP_4);
	m_input.ObserveKey(GLFW_KEY_KP_5);
	m_input.ObserveKey(GLFW_KEY_KP_6);
	m_input.ObserveKey(GLFW_KEY_KP_8);
	m_input.ObserveKey(GLFW_KEY_LEFT_SHIFT);
	m_input.ObserveKey(GLFW_KEY_P);

	m_forwardVector = FORWARD_AXIS;
	m_rightVector = RIGHT_AXIS;
	m_upVector = UP_AXIS;

	m_cubieOffset = m_cubieRenderer.GetCubieExtension();

	for(int x = 0; x < CUBE_SIZE; x++)
		for(int y = 0; y < CUBE_SIZE; y++)
			for (int z = 0; z < CUBE_SIZE; z++)
			{
				glm::vec3 localPosition = (glm::vec3(x, y, z) - (CUBE_SIZE-1) / 2.0f) * m_cubieOffset;
				m_transform[x][y][z] = glm::mat4(1.0f);
				m_transform[x][y][z] = glm::translate(m_transform[x][y][z], localPosition);
			}
}

void ThreeXThreeCube::Render(float aspectRatio)
{
	glm::mat4 projection = glm::perspective(glm::radians(70.0f), aspectRatio, 0.1f, 100.0f);
	glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 8.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	for (int x = 0; x < CUBE_SIZE; x++)
		for (int y = 0; y < CUBE_SIZE; y++)
			for (int z = 0; z < CUBE_SIZE; z++)
			{
				glm::mat4 model = m_transform[x][y][z];
				m_cubieRenderer.Render(projection, view, model);
			}
}

void ThreeXThreeCube::ClearResource()
{
	m_cubieRenderer.ClearResources();
}

void ThreeXThreeCube::Update(double deltaTime)
{
	m_input.Update();
	UpdateMouseData();

	/*---Rotate whole Cube---*/
	glm::vec3 euler = glm::vec3(0.0f, 0.0f, 0.0f);

	if (m_input.RightMouseDown())
	{
		euler.x = m_mouseVelY * MOUSE_SENSITIVITY * deltaTime;
		euler.y = m_mouseVelX * MOUSE_SENSITIVITY * deltaTime;
	}

	if (m_input.IsDown(GLFW_KEY_W))
	{
		euler.x += glm::radians(-ROTATION_SPEED);
	}

	if (m_input.IsDown(GLFW_KEY_S))
	{
		euler.x += glm::radians(ROTATION_SPEED);
	}

	if (m_input.IsDown(GLFW_KEY_D))
	{
		euler.y += glm::radians(ROTATION_SPEED);
	}

	if (m_input.IsDown(GLFW_KEY_A))
	{
		euler.y += glm::radians(-ROTATION_SPEED);
	}

	if (m_input.IsDown(GLFW_KEY_E))
	{
		euler.z += glm::radians(-ROTATION_SPEED);
	}

	if (m_input.IsDown(GLFW_KEY_Q))
	{
		euler.z += glm::radians(ROTATION_SPEED);
	}

	if (!m_rotating)
	{
		RotateRubiksCube(euler * (float)deltaTime);
	}


	/*---Rotate Sides---*/
	if (!m_rotating)
	{
		m_rotationDirection = 1;
		if (m_input.IsDown(GLFW_KEY_LEFT_SHIFT))
		{
			m_rotationDirection = -1;
		}

		if (m_input.Pressed(GLFW_KEY_KP_2) || m_input.Pressed(GLFW_KEY_KP_4) || m_input.Pressed(GLFW_KEY_KP_5) || m_input.Pressed(GLFW_KEY_KP_6) || m_input.Pressed(GLFW_KEY_KP_8) ||
			m_input.Pressed(GLFW_KEY_COMMA) || m_input.Pressed(GLFW_KEY_J) || m_input.Pressed(GLFW_KEY_K) || m_input.Pressed(GLFW_KEY_L) || m_input.Pressed(GLFW_KEY_I))
		{
			m_rotating = true;
			PlayTwistSound();
		}

		if (m_input.Pressed(GLFW_KEY_KP_2) || m_input.Pressed(GLFW_KEY_COMMA))
		{
			m_rotatingFace = Face::BOTTOM;
			m_cubiesOnCurrentFace = GetCubiesOnFace(m_rotatingFace);
		}
		else if (m_input.Pressed(GLFW_KEY_KP_4) || m_input.Pressed(GLFW_KEY_J))
		{
			m_rotatingFace = Face::LEFT;
			m_cubiesOnCurrentFace = GetCubiesOnFace(m_rotatingFace);
		}
		else if (m_input.Pressed(GLFW_KEY_KP_5) || m_input.Pressed(GLFW_KEY_K))
		{
			m_rotatingFace = Face::FRONT;
			m_cubiesOnCurrentFace = GetCubiesOnFace(m_rotatingFace);
		}
		else if (m_input.Pressed(GLFW_KEY_KP_6) || m_input.Pressed(GLFW_KEY_L))
		{
			m_rotatingFace = Face::RIGHT;
			m_cubiesOnCurrentFace = GetCubiesOnFace(m_rotatingFace);
		}
		else if (m_input.Pressed(GLFW_KEY_KP_8) || m_input.Pressed(GLFW_KEY_I))
		{
			m_rotatingFace = Face::TOP;
			m_cubiesOnCurrentFace = GetCubiesOnFace(m_rotatingFace);
		}
	}
	else if (m_rotating && m_degreesRotated < 90.0f)
	{
		float rotationToAdd = m_rotationDirection * -TWIST_SPEED * deltaTime;
		RotateFace(m_rotatingFace, m_cubiesOnCurrentFace, rotationToAdd);
		m_degreesRotated += abs(TWIST_SPEED * deltaTime);
	}
	else
	{
		float correctionTurn = m_rotationDirection * (m_degreesRotated - 90.0f);
		RotateFace(m_rotatingFace, m_cubiesOnCurrentFace, correctionTurn);
		m_degreesRotated = 0.0f;
		m_rotating = false;
	}
}

void ThreeXThreeCube::PrintMat4(const glm::mat4& mat)
{
	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			std::cout << mat[y][x] << "   ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl << "---------------------------------------------" << std::endl;
}

void ThreeXThreeCube::PrintVec(const glm::vec4& vec)
{
	std::cout << vec.x << "   " << vec.y << "   " << vec.z << std::endl;
}

void ThreeXThreeCube::PrintVec(const glm::vec3& vec)
{
	std::cout << vec.x << "   " << vec.y << "   " << vec.z << std::endl;
}

void ThreeXThreeCube::RotateCubie(const glm::vec3 axis, const float degrees, glm::mat4& cubieTransform)
{
	float cosAng = glm::cos(glm::radians(degrees) / 2.0f);
	float sinAng = glm::sin(glm::radians(degrees) / 2.0f);
	glm::quat rotationQuaternion = glm::quat(cosAng, sinAng * axis.x, sinAng * axis.y, sinAng * axis.z);
	glm::mat4 rotationMatrix = glm::mat4_cast(rotationQuaternion);
	cubieTransform = rotationMatrix * cubieTransform;
}

void ThreeXThreeCube::RotateRubiksCube(const glm::vec3 euler)
{
	glm::quat rotationQuaternion = glm::quat(euler);
	glm::mat4 rotationMatrix = glm::mat4_cast(rotationQuaternion);

	for (int x = 0; x < CUBE_SIZE; x++)
		for (int y = 0; y < CUBE_SIZE; y++)
			for (int z = 0; z < CUBE_SIZE; z++)
			{
				m_transform[x][y][z] = rotationMatrix * m_transform[x][y][z];
			}

	m_forwardVector = rotationMatrix * m_forwardVector;
	m_rightVector = rotationMatrix * m_rightVector;
	m_upVector = rotationMatrix * m_upVector;
}

void ThreeXThreeCube::RotateFace(Face face, const std::vector<glm::mat4*>& cubiesOnFace, float degree)
{
	glm::vec3 cameraRelativeForward;
	glm::vec3 cameraRelativeRight;
	glm::vec3 cameraRelativeUp;
	GetCameraRelativeDirectionVectors(cameraRelativeForward, cameraRelativeRight, cameraRelativeUp);

	for (int i = 0; i < m_cubiesOnCurrentFace.size(); i++)
	{
		switch (m_rotatingFace)
		{
		case FRONT:
			RotateCubie(cameraRelativeForward, degree, *cubiesOnFace[i]);
			break;
		case BACK:
			RotateCubie(cameraRelativeForward, degree, *cubiesOnFace[i]);
			break;
		case RIGHT:
			RotateCubie(cameraRelativeRight, degree, *cubiesOnFace[i]);
			break;
		case LEFT:
			RotateCubie(cameraRelativeRight, degree, *cubiesOnFace[i]);
			break;
		case TOP:
			RotateCubie(cameraRelativeUp, degree, *cubiesOnFace[i]);
			break;
		case BOTTOM:
			RotateCubie(cameraRelativeUp, degree, *cubiesOnFace[i]);
			break;
		default:
			break;
		}
	}
}

void ThreeXThreeCube::GetCameraRelativeDirectionVectors(glm::vec3& outForward, glm::vec3& outRight, glm::vec3& outUp)
{
	std::vector<glm::vec3> directionVectors = { m_forwardVector, m_rightVector, m_upVector };
	int indexLastSetVector = 0;

	//Set camera relative forward vector
	for (int i = 0; i < 3; i++)
	{
		if (glm::abs(directionVectors[i].z) > outForward.z)
		{
			if (directionVectors[i].z < 0)
			{
				directionVectors[i] *= -1;
			}

			outForward = directionVectors[i];
			indexLastSetVector = i;
		}
	}

	directionVectors.erase(directionVectors.begin() + indexLastSetVector);

	//Set camera relative right vector
	for (int i = 0; i < 2; i++)
	{
		if (glm::abs(directionVectors[i].x) > outRight.x)
		{
			if (directionVectors[i].x < 0)
			{
				directionVectors[i] *= -1;
			}

			outRight = directionVectors[i];
			indexLastSetVector = i;
		}
	}

	directionVectors.erase(directionVectors.begin() + indexLastSetVector);

	//Set camera relative up vector
	if (directionVectors[0].y < 0)
	{
		directionVectors[0] *= -1;
	}

	outUp = directionVectors[0];
}

std::vector<glm::mat4*> ThreeXThreeCube::GetCubiesOnFace(Face face)
{
	std::vector<glm::mat4*> cubies;

	glm::vec3 cameraRelativeForward;
	glm::vec3 cameraRelativeRight;
	glm::vec3 cameraRelativeUp;
	GetCameraRelativeDirectionVectors(cameraRelativeForward, cameraRelativeRight, cameraRelativeUp);

	for (int x = 0; x < CUBE_SIZE; x++) 
		for (int y = 0; y < CUBE_SIZE; y++)
			for (int z = 0; z < CUBE_SIZE; z++)
			{
				for (float i = -1; i < 2; i++)
					for (float j = -1; j < 2; j++)
					{
						glm::vec3 cubiePos = m_transform[x][y][z][3];

						switch (face)
						{
						case FRONT:
							if (EqualWithError(cubiePos, cameraRelativeForward * m_cubieOffset + i * cameraRelativeRight * m_cubieOffset + j * cameraRelativeUp * m_cubieOffset, 0.001f))
							{
								cubies.push_back(&m_transform[x][y][z]);
							}
							break;
						case BACK:
							if (EqualWithError(cubiePos, -cameraRelativeForward * m_cubieOffset + i * cameraRelativeRight * m_cubieOffset + j * cameraRelativeUp * m_cubieOffset, 0.001f))
							{
								cubies.push_back(&m_transform[x][y][z]);
							}
							break;
						case RIGHT:
							if (EqualWithError(cubiePos, cameraRelativeRight * m_cubieOffset + i * cameraRelativeForward * m_cubieOffset + j * cameraRelativeUp * m_cubieOffset, 0.001f))
							{
								cubies.push_back(&m_transform[x][y][z]);
							}
							break;
						case LEFT:
							if (EqualWithError(cubiePos, -cameraRelativeRight * m_cubieOffset + i * cameraRelativeForward * m_cubieOffset + j * cameraRelativeUp * m_cubieOffset, 0.001f))
							{
								cubies.push_back(&m_transform[x][y][z]);
							}
							break;
						case TOP:
							if (EqualWithError(cubiePos, cameraRelativeUp * m_cubieOffset + i * cameraRelativeForward * m_cubieOffset + j * cameraRelativeRight * m_cubieOffset, 0.001f))
							{
								cubies.push_back(&m_transform[x][y][z]);
							}
							break;
						case BOTTOM:
							if (EqualWithError(cubiePos, -cameraRelativeUp * m_cubieOffset + i * cameraRelativeForward * m_cubieOffset + j * cameraRelativeRight * m_cubieOffset, 0.001f))
							{
								cubies.push_back(&m_transform[x][y][z]);
							}
							break;
						default:
							break;
						}
					}
			}
	return cubies;
}

void ThreeXThreeCube::UpdateMouseData()
{
	m_input.GetMousePosition(m_mousePosX, m_mousePosY);

	int screenWidth, screenHeight;
	glfwGetFramebufferSize(m_window, &screenWidth, &screenHeight);
	float aspectRatio = (float)screenWidth / (float)screenHeight;

	m_mouseVelX = m_mousePosX - m_mousePosOldX;
	m_mouseVelY = (m_mousePosY - m_mousePosOldY) / aspectRatio;

	m_mousePosOldX = m_mousePosX;
	m_mousePosOldY = m_mousePosY;
}

bool ThreeXThreeCube::EqualWithError(glm::vec3 a, glm::vec3 b, float allowedError)
{

	return (a.x <= b.x + allowedError && a.y <= b.y + allowedError && a.z <= b.z + allowedError) && (a.x >= b.x - allowedError && a.y >= b.y - allowedError && a.z >= b.z - allowedError);
}

void ThreeXThreeCube::PlayTwistSound()
{
	std::random_device rand_dev;
	std::mt19937 generator(rand_dev());
	std::uniform_int_distribution<int> distr(0, 27);
	int randomSound = distr(generator);

	if (randomSound == 0)
	{
		PlaySound(TEXT("TwistFX_0.wav"), NULL, SND_ASYNC);
	}
	else if (randomSound == 1)
	{
		PlaySound(TEXT("TwistFX_1.wav"), NULL, SND_ASYNC);
	}
	else if (randomSound == 2)
	{
		PlaySound(TEXT("TwistFX_2.wav"), NULL, SND_ASYNC);
	}
	else if (randomSound == 3)
	{
		PlaySound(TEXT("TwistFX_3.wav"), NULL, SND_ASYNC);
	}
	else if (randomSound == 4)
	{
		PlaySound(TEXT("TwistFX_4.wav"), NULL, SND_ASYNC);
	}
	else if (randomSound == 5)
	{
		PlaySound(TEXT("TwistFX_5.wav"), NULL, SND_ASYNC);
	}
	else if (randomSound == 6)
	{
		PlaySound(TEXT("TwistFX_6.wav"), NULL, SND_ASYNC);
	}
	else if (randomSound == 7)
	{
		PlaySound(TEXT("TwistFX_7.wav"), NULL, SND_ASYNC);
	}
	else if (randomSound == 8)
	{
		PlaySound(TEXT("TwistFX_8.wav"), NULL, SND_ASYNC);
	}
	else if (randomSound == 9)
	{
		PlaySound(TEXT("TwistFX_9.wav"), NULL, SND_ASYNC);
	}
	else if (randomSound == 10)
	{
		PlaySound(TEXT("TwistFX_10.wav"), NULL, SND_ASYNC);
	}
	else if (randomSound == 11)
	{
		PlaySound(TEXT("TwistFX_11.wav"), NULL, SND_ASYNC);
	}
	else if (randomSound == 12)
	{
		PlaySound(TEXT("TwistFX_12.wav"), NULL, SND_ASYNC);
	}
	else if (randomSound == 13)
	{
		PlaySound(TEXT("TwistFX_13.wav"), NULL, SND_ASYNC);
	}
	else if (randomSound == 14)
	{
		PlaySound(TEXT("TwistFX_14.wav"), NULL, SND_ASYNC);
	}
	else if (randomSound == 15)
	{
		PlaySound(TEXT("TwistFX_15.wav"), NULL, SND_ASYNC);
	}
	else if (randomSound == 16)
	{
		PlaySound(TEXT("TwistFX_16.wav"), NULL, SND_ASYNC);
	}
	else if (randomSound == 17)
	{
		PlaySound(TEXT("TwistFX_17.wav"), NULL, SND_ASYNC);
	}
	else if (randomSound == 18)
	{
		PlaySound(TEXT("TwistFX_18.wav"), NULL, SND_ASYNC);
	}
	else if (randomSound == 19)
	{
		PlaySound(TEXT("TwistFX_19.wav"), NULL, SND_ASYNC);
	}
	else if (randomSound == 20)
	{
		PlaySound(TEXT("TwistFX_20.wav"), NULL, SND_ASYNC);
	}
	else if (randomSound == 21)
	{
		PlaySound(TEXT("TwistFX_21.wav"), NULL, SND_ASYNC);
	}
	else if (randomSound == 22)
	{
		PlaySound(TEXT("TwistFX_22.wav"), NULL, SND_ASYNC);
	}
	else if (randomSound == 23)
	{
		PlaySound(TEXT("TwistFX_23.wav"), NULL, SND_ASYNC);
	}
	else if (randomSound == 24)
	{
		PlaySound(TEXT("TwistFX_24.wav"), NULL, SND_ASYNC);
	}
	else if (randomSound == 25)
	{
		PlaySound(TEXT("TwistFX_25.wav"), NULL, SND_ASYNC);
	}
	else if (randomSound == 26)
	{
		PlaySound(TEXT("TwistFX_26.wav"), NULL, SND_ASYNC);
	}
	else if (randomSound == 27)
	{
		PlaySound(TEXT("TwistFX_27.wav"), NULL, SND_ASYNC);
	}
}
