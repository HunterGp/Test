// RubiksCube.cpp : This file contains the 'main' function. Program execution begins and ends there.

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "GameInterface.h"
#include "ThreeXThreeCube.h"



GameInterface gDummyTest;
ThreeXThreeCube gTwoXTwoCube;

GameInterface* gUsedInterface;

GLFWwindow* InitializeSystem()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

	GLFWwindow* window = glfwCreateWindow(1024, 768, "Rubiks Cube", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glewExperimental = true;
	glewInit();

	gUsedInterface->Initialize(window);

	return window;
}

void RunCoreLoop(GLFWwindow* window)
{
	double lastTime = glfwGetTime();
	double timeDifference = 0.0;

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		gUsedInterface->Update(timeDifference);

		int screenWidth, screenHeight;
		glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
		float aspectRatio = (float)screenWidth / (float)screenHeight;

		glViewport(0, 0, screenWidth, screenHeight);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		gUsedInterface->Render(aspectRatio);
		glfwSwapBuffers(window);

		double currentTime = glfwGetTime();
		timeDifference = currentTime - lastTime;
		lastTime = currentTime;
	}
}

void ShutdownSystem()
{
	gUsedInterface->ClearResource();
	glfwTerminate();
}

int main()
{
	gUsedInterface = &gTwoXTwoCube;
	GLFWwindow* window = InitializeSystem();
	RunCoreLoop(window);
	ShutdownSystem();
}