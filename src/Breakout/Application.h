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
	void Update(GLFWwindow* window, float dt);
	void Render();

	void ProcessInput(GLFWwindow* window);
	void ProcessCameras(GLFWwindow* window);

	void BuildLevel();
	void UpdateCameraView(unsigned view);
	bool IsTheGameWon();
	
	void UpdatePlayerPosition(GLFWwindow* window);
	void UpdateBallPosition(GLFWwindow* window);
	
	bool CollisionDetection(std::unique_ptr<Ball>& ball, std::unique_ptr<Brick>& brick);
	bool CollisionDetection(std::unique_ptr<Ball>& ball, std::unique_ptr<Player>& player);

	void SetCrackedBrick(int x, int y);
	void SetDeadBrick(int x, int y);
	void SetDyingBrick(int x, int y);

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