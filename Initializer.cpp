#pragma once
#include "pch.h"

#ifdef __gl_h_
#undef __gl_h_
#endif // __gl_h_
#include <glad/glad.h>

#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "Initializer.h"
#include "EventsController.h"
#include "LightingController.h"
#include <Screen.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

bool InitializationHandlerSingleton::Init(Vector2 windowSize)
{
	if (!glfwInit())
	{
		cout << "glfwInit failed" << endl;
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	window = glfwCreateWindow(windowSize.x, windowSize.y, "Game", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		cout << "glfwCreateWindow failed" << endl;
		return false;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGL())
	{
		cout << "gladLoadGL failed" << endl;
		return false;
	}

	cout << "Renderer: " << glGetString(GL_RENDERER) << endl;
	cout << "OpenGL Version: " << glGetString(GL_VERSION) << endl;

	// glad: load all OpenGL function pointers×
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	glfwSetKeyCallback(window, 
		[](GLFWwindow* window, int key, int scancode, int action, int mode) 
		{ 
			if (EventsController->enableKeyboardEvent)
				EventsController->KeyboardEvent(window, key, scancode, action, mode);
		});
	glfwSetCursorPosCallback(window, 
		[](GLFWwindow* window, double x, double y)
		{
			EventsController->MouseMoveEvent(window, x, y);
		});
	glfwSetMouseButtonCallback(window, 
		[](GLFWwindow* window, int button, int action, int mods)
		{
			EventsController->MouseButtonEvent(window, button, action, mods);
		});
	glfwSetScrollCallback(window, 
		[](GLFWwindow* window, double xoffset, double yoffset)
		{
			if (EventsController->enableMouseWheelEvent)
				EventsController->MouseWheelEvent(window, xoffset, yoffset);
		});
	glfwSetFramebufferSizeCallback(window, 
		[](GLFWwindow* window, int width, int height)
		{
			EventsController->WindowResizeEvent(window, width, height);
		});

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(NULL);
	ImGui::StyleColorsDark();

	Screen->windowSize = windowSize;
	Screen->window = window;

	LightingController::Initialize();

	return true;
}

void InitializationHandlerSingleton::Quit()
{ 
	glfwTerminate(); 
}

GLFWwindow* InitializationHandlerSingleton::GetWindow()
{
	return window;
}
