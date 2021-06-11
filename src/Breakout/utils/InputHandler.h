#pragma once

#include <unordered_map>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

class InputHandler
{
public:
	InputHandler();
	~InputHandler();

	void update();

	void keyPressed(unsigned int keyID);
	void keyReleased(unsigned int keyID);

	bool isKeyDown(unsigned int keyID);
	bool isKeyPressed(unsigned int keyID);

	void mousePosition(float x, float y);
	
	glm::vec2 mousePosition(GLFWwindow* window, bool setPos, int screenWidth, int screenHeight);
	glm::vec2 mousePosition() const;
	
	void mouseWheel(int wheelY);
	int mouseWheel() const;

private:
	// returns if a key was previously down (last frame)
	bool wasKeyDown(unsigned int keyID);
	
	std::unordered_map<unsigned int, bool> keyMap;
	std::unordered_map<unsigned int, bool> previousKeyMap;

	glm::vec2 mousePos;
	glm::vec2 lastMousePos;
	
	int wheelPos;
};