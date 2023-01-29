#include "pch.h"

#ifdef __gl_h_
#undef __gl_h_
#endif // __gl_h_
#include <glad/glad.h>

#include <EventSystem.h>
#include <Screen.h>
#include "EventsController.h"
#include "GameObject.h"

void EventsControllerSingleton::SetAsMainCamera(CameraComponent* cam)
{
	EventSystem->mainCamera = cam;
}

void EventsControllerSingleton::SetPlayMode(bool inPlayMode)
{
	EventSystem->inPlayMode = inPlayMode;
}

void EventsControllerSingleton::Update()
{
	// setting last frame inputs
	EventSystem->leftMouseButtonLastFrame = EventSystem->leftMouseButton;
	EventSystem->rightMouseButtonLastFrame = EventSystem->rightMouseButton;

	if (EventSystem->inPlayMode)
	{
		Component* currentComp;
		for (auto it = EventSystem->onUpdate.begin(); it != EventSystem->onUpdate.end(); it++)
		{
			currentComp = reinterpret_cast<Component*>(it->first);
			if (currentComp && currentComp->gameObject->isActive)
				it->second();
		}
	}
	else
		for (auto& it : EventSystem->onUpdateEditor)
			it();

	EventSystem->mouseWheelOffset.x = 0;
	EventSystem->mouseWheelOffset.y = 0;
	EventSystem->mouseMotion.x = 0;
	EventSystem->mouseMotion.y = 0;
	for (int i = 0; i < GLFW_KEY_LAST; i++)
		EventSystem->keysLastFrame[i] = EventSystem->keys[i];
}
void EventsControllerSingleton::MouseMoveEvent(GLFWwindow* window, double x, double y)
{
	EventSystem->prevMousePos = EventSystem->mousePos;
	EventSystem->mousePos.x = x;
	EventSystem->mousePos.y = y;
	EventSystem->mouseMotion = EventSystem->prevMousePos - EventSystem->mousePos;

	if (EventSystem->inPlayMode)
	{
		Component* currentComp;
		for (auto it = EventSystem->onMouseMoveEvent.begin(); it != EventSystem->onMouseMoveEvent.end(); it++)
		{
			currentComp = reinterpret_cast<Component*>(it->first);
			if (currentComp && currentComp->gameObject->isActive)
				it->second(window, x, y, EventSystem->mouseMotion);
		}
	}
	else
		for (auto& it : EventSystem->onMouseMoveEventEditor)
			it(window, x, y, EventSystem->mouseMotion);
}
void EventsControllerSingleton::MouseButtonEvent(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS)
			EventSystem->leftMouseButton = true;
		else if (action == GLFW_RELEASE)
			EventSystem->leftMouseButton = false;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		if (action == GLFW_PRESS)
			EventSystem->rightMouseButton = true;
		else if (action == GLFW_RELEASE)
			EventSystem->rightMouseButton = false;
	}

	if (EventSystem->inPlayMode)
	{
		Component* currentComp;
		for (auto it = EventSystem->onMouseButtonEvent.begin(); it != EventSystem->onMouseButtonEvent.end(); it++)
		{
			currentComp = reinterpret_cast<Component*>(it->first);
			if (currentComp && currentComp->gameObject->isActive)
				it->second(window, button, action, mods);
		}
	}
	else
		for (auto& it : EventSystem->onMouseButtonEventEditor)
			it(window, button, action, mods);
}
void EventsControllerSingleton::MouseWheelEvent(GLFWwindow* window, double xoffset, double yoffset)
{
	EventSystem->mouseWheelOffset.x = xoffset;
	EventSystem->mouseWheelOffset.y = yoffset;

	if (EventSystem->inPlayMode)
	{
		Component* currentComp;
		for (auto it = EventSystem->onMouseWheelEvent.begin(); it != EventSystem->onMouseWheelEvent.end(); it++)
		{
			currentComp = reinterpret_cast<Component*>(it->first);
			if (currentComp && currentComp->gameObject->isActive)
				it->second(window, xoffset, yoffset);
		}
	}
	else
		for (auto& it : EventSystem->onMouseWheelEventEditor)
			it(window, xoffset, yoffset);
}
void EventsControllerSingleton::KeyboardEvent(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key < 0)
		return;

	if (action == GLFW_PRESS)
		EventSystem->keys[key] = true;
	else if (action == GLFW_RELEASE)
		EventSystem->keys[key] = false;

	if (EventSystem->inPlayMode)
	{
		Component* currentComp;
		for (auto it = EventSystem->onKeyboardEvent.begin(); it != EventSystem->onKeyboardEvent.end(); it++)
		{
			currentComp = reinterpret_cast<Component*>(it->first);
			if (currentComp && currentComp->gameObject->isActive)
				it->second(window, key, scancode, action, mode);
		}
	}
	else
		for (auto& it : EventSystem->onKeyboardEventEditor)
			it(window, key, scancode, action, mode);
}
void EventsControllerSingleton::WindowResizeEvent(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	Screen->windowSize = Vector2(width, height);

	if (EventSystem->inPlayMode)
	{
		Component* currentComp;
		for (auto it = EventSystem->onWindowResizeEvent.begin(); it != EventSystem->onWindowResizeEvent.end(); it++)
		{
			currentComp = reinterpret_cast<Component*>(it->first);
			if (currentComp && currentComp->gameObject->isActive)
				it->second(window, width, height);
		}
	}
	else
		for (auto& it : EventSystem->onWindowResizeEventEditor)
			it(window, width, height);
}