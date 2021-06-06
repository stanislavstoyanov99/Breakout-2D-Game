#pragma once

#include <glm/glm.hpp>

#include "Shader.h"
#include "Texture.h"
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
	std::unique_ptr<Shader> _shader;
	std::unique_ptr<Shader> _lightCubeShader;

	std::unique_ptr<Texture> _diffuseTexture;
	std::unique_ptr<Texture> _specularTexture;

	glm::vec3 _lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
};