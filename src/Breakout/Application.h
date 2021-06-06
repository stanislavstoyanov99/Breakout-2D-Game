#pragma once

#include <glm/glm.hpp>

#include "VertexArray.h"

struct GLFWwindow;

class Application
{
public:
	void Run();

private:
	void Init();
	void Update(float dt);
	void Render();

	void ProcessInput(GLFWwindow* window);

	friend void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	friend void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	std::unique_ptr<VertexArray> _va;

	glm::vec3 _lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
	glm::vec3 _lightColor = glm::vec3(1.f, 1.f, 1.f);
};