#pragma once

#include <glm/glm.hpp>

#include "Shader.h"
#include "VertexArray.h"

#include "models/Ball.h"
#include "models/Brick.h"
#include "models/Model.h"
#include "models/Player.h"
#include "models/GameObject.h"

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
	void ProcessCameras(GLFWwindow* window);

	friend void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	friend void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	void BuildLevel();
	void UpdateCameraView(unsigned view);

	std::unique_ptr<Shader> _shader;

	std::unique_ptr<GameObject> _skybox;
	std::unique_ptr<Player> _player;
	std::unique_ptr<Ball> _ball;
	
	std::unique_ptr<Brick> _brick;
	std::unique_ptr<Brick> _brickLeft;
	std::unique_ptr<Brick> _brickRight;
	std::unique_ptr<Brick> _brickTop;

	glm::vec3 _lightPos = glm::vec3(30.0f, 30.0f, 30.0f);
};