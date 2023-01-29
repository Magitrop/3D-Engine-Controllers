#pragma once
#include "pch.h"
#include <GLFW\glfw3.h>
#include <Singleton.h>

#define EventsController SingletonManager::Instance()->Get<EventsControllerSingleton>()

class CameraComponent;
class EventsControllerSingleton final : SingletonBase
{
	SINGLETON(EventsControllerSingleton);
private:
	friend class EngineEditor;
	friend class InitializationHandlerSingleton;

	bool enableKeyboardEvent = true;
	bool enableMouseWheelEvent = true;
public:
	void SetAsMainCamera(CameraComponent* cam);
	void SetPlayMode(bool inPlayMode);

	void Update();
	void MouseMoveEvent(GLFWwindow* window, double x, double y);
	void MouseButtonEvent(GLFWwindow* window, int button, int action, int mods);
	void MouseWheelEvent(GLFWwindow* window, double xoffset, double yoffset);
	void KeyboardEvent(GLFWwindow* window, int key, int scancode, int action, int mode);
	void WindowResizeEvent(GLFWwindow* window, int width, int height);
};