#pragma once
#include "pch.h"

#include <iostream>
#include <map>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <Singleton.h>

using glm::Vector2;
using glm::Vector3;
using glm::Vector4;
using std::cout;
using std::endl;

#define InitializationHandler SingletonManager::Instance()->Get<InitializationHandlerSingleton>()

class InitializationHandlerSingleton final : SingletonBase
{
	SINGLETON(InitializationHandlerSingleton);

	friend class EngineEditor;
	GLFWwindow* window;
public:
	bool Init(Vector2 windowSize);
	void Quit();

	GLFWwindow* GetWindow();
};