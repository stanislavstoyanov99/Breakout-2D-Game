#include "InputHandler.h"

#include <GLFW/glfw3.h>

InputHandler::InputHandler()
	: mousePos(0), lastMousePos(0), wheelPos(0) {}

InputHandler::~InputHandler() {}

void InputHandler::update()
{
	for (auto& it : keyMap)
	{
		previousKeyMap[it.first] = it.second;	
	}

	wheelPos = 0;
}

void InputHandler::keyPressed(unsigned int keyID)
{
	keyMap[keyID] = true;
}

void InputHandler::keyReleased(unsigned int keyID)
{
	keyMap[keyID] = false;
}

bool InputHandler::isKeyDown(unsigned int keyID)
{
	auto it = keyMap.find(keyID);

	if (it != keyMap.end())
	{
		return it->second;
	}

	return false;
}

bool InputHandler::isKeyPressed(unsigned int keyID)
{
	if (isKeyDown(keyID) && !wasKeyDown(keyID))
	{
		return true;
	}
	
	return false;
}

void InputHandler::mousePosition(float x, float y)
{
	lastMousePos = mousePos;
	mousePos.x = x;
	mousePos.y = y;
}

glm::vec2 InputHandler::mousePosition(GLFWwindow* window, bool setPos, int screenWidth, int screenHeight)
{
	float xoffset = 0;
	float yoffset = 0;

	if (setPos)
	{
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		
		xoffset = (float)(x - (screenWidth / 2));
		yoffset = (float)(y - (screenHeight / 2));
		
		lastMousePos = mousePos;
	}

	lastMousePos = mousePos;
	
	return glm::vec2(xoffset, yoffset);
}

glm::vec2 InputHandler::mousePosition() const
{
	return mousePos;
}

void InputHandler::mouseWheel(int wheelY)
{
	wheelPos = wheelY;
}

int InputHandler::mouseWheel() const
{
	return wheelPos;
}

bool InputHandler::wasKeyDown(unsigned int keyID)
{
	auto it = previousKeyMap.find(keyID);
	
	if (it != previousKeyMap.end())
	{
		return it->second;
	}

	return false;
}