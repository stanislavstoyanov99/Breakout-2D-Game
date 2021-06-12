#include "Application.h"

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "Settings.h"
#include "Shader.h"

#include "models/Ball.h"
#include "models/Brick.h"
#include "models/Player.h"
#include "models/GameObject.h"
#include "utils/InputHandler.h"

// game states
enum class GameState { Play, Win, Lose, Exit };
GameState state;

// game elements
InputHandler inputHandler;

// camera initialization
Camera camera(glm::vec3(0.0f, 0.0f, 28.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

// camera views
glm::vec3 firstCamView = glm::vec3(0.0f, -15.0f, 15.0f);
glm::vec3 secondCamView = glm::vec3(0.0f, 15.0f, 20.0f);
glm::vec3 thirdCamView = glm::vec3(25.0f, 0.0f, 20.0f);;
glm::vec3 fourthCamView = camera.Position;

// camera variables
bool updateView;
bool freeCamera;

// screen dimensions
int screenWidth = SCR_WIDTH;
int screenHeight = SCR_HEIGHT;

// ball
glm::vec3 ballVelocity = { 5.5f, 10.0f, 0.0f };
bool stuckToPaddle;
float offset = 0;

// player
glm::vec3 playerVelocity = { 15.0f, 0.0f, 0.0f };
int score;

// bricks
const unsigned numbBricksHigh = 5;
const unsigned numbBricksWide = 10;
const unsigned boundBlocks = 20;
const unsigned topBlocks = 25;
std::unique_ptr<Brick> bricks[numbBricksHigh][numbBricksWide];
std::unique_ptr<Brick> boundLeft[boundBlocks];
std::unique_ptr<Brick> boundRight[boundBlocks];
std::unique_ptr<Brick> boundTop[topBlocks];

// variables
bool gameWon;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void Application::ProcessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		state = GameState::Exit;
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		inputHandler.keyPressed(glfwGetKey(window, GLFW_KEY_DOWN));
	}

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		inputHandler.keyReleased(glfwGetKey(window, GLFW_KEY_UP));
	}
}

void Application::ProcessCameras(GLFWwindow* window)
{
	unsigned cam = 0;

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		cam = 1;
		updateView = true;
	}

	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		cam = 2;
		updateView = true;
	}

	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
	{
		cam = 3;
		updateView = true;
	}

	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
	{
		cam = 4;
		updateView = true;
	}

	if (freeCamera)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		// set mouse to the center of the screen
		glfwSetCursorPos(window, screenWidth / 2, screenHeight / 2);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			camera.ProcessKeyboard(FORWARD, deltaTime);
		}

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			camera.ProcessKeyboard(BACKWARD, deltaTime);
		}

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			camera.ProcessKeyboard(LEFT, deltaTime);
		}

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			camera.ProcessKeyboard(RIGHT, deltaTime);
		}

		// TODO: fix bug with mouse movement
		if (inputHandler.isKeyDown(GLFW_MOUSE_BUTTON_RIGHT))
		{
			const auto mousePos = inputHandler.mousePosition(window, true, screenWidth, screenHeight);
			camera.ProcessMouseMovement(mousePos.x, mousePos.y, true);
		}

		if (inputHandler.mouseWheel() != 0)
		{
			if (camera.Zoom >= 1.0f && camera.Zoom <= 45.0f)
			{
				camera.Zoom += inputHandler.mouseWheel();
			}

			if (camera.Zoom <= 1.0f)
			{
				camera.Zoom = 1.0f;
			}

			if (camera.Zoom >= 45.0f)
			{
				camera.Zoom = 45.0f;
			}
		}
	}
	else
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	inputHandler.update();

	camera.UpdateVectors();

	if (updateView)
	{
		UpdateCameraView(cam);
	}
}

void Application::Run()
{
	// glfw: initialize and configure
	if (!glfwInit())
		return;

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, WINDOW_TITLE, NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		return;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}

	glViewport(0, 0, screenWidth, screenHeight);

	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	Init();

	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		const float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// handle input
		ProcessInput(window);

		// handle cameras switch
		ProcessCameras(window);

		// handle updating
		Update(window, deltaTime);

		// handle rendering
		Render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
}

void Application::Init()
{
	state = GameState::Play;
	gameWon = false;

	updateView = false;
	freeCamera = false;

	// ball stuck to paddle
	stuckToPaddle = true;

	// player score
	score = 0;

	_shader = std::make_unique<Shader>("res\\projection.vert.glsl", "res\\projection.frag.glsl");

	// load skybox model
	{
		_skybox = std::make_unique<GameObject>("res\\models\\sky\\sky.obj");

		_skybox->position = glm::vec3(0.0f);
		_skybox->scale = glm::vec3(100.0f);
	}

	// load player model
	{
		_player = std::make_unique<Player>("res\\models\\player\\player.obj");

		_player->position = glm::vec3(0.0f, -7.5f, 0.0f);
		_player->scale = glm::vec3(1.5f, 0.125f, 0.5f);

		offset = _player->scale.x;
	}

	// load ball model
	{
		_ball = std::make_unique<Ball>("res\\models\\ball\\ball.obj");

		_ball->position = glm::vec3(_player->position.x, _player->position.y + _ball->scale.y, _player->position.z);
		_ball->scale = glm::vec3(0.35f, 0.35f, 0.35f);
	}

	// level
	{
		BuildLevel();
	}

	glEnable(GL_DEPTH_TEST);
}

void Application::Update(GLFWwindow* window, float dt)
{
	const auto viewMatrix = camera.GetViewMatrix();

	const auto projectionMatrix =
		glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);

	_shader->use();
	_shader->setFloatMat4("view", viewMatrix);
	_shader->setFloatMat4("projection", projectionMatrix);
	
	if (state == GameState::Play)
	{
		gameWon = IsTheGameWon();
		
		if (gameWon)
		{
			state = GameState::Win;
		}

		// update the paddle
		UpdatePlayerPosition(window);

		// update the ball
		UpdateBallPosition(window);
	}
}

void Application::Render()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_shader->use();

	_shader->setFloat3("viewPos", camera.Position);

	_shader->setFloat("material.shininess", 64.0f);

	_shader->setFloat3("light.position", _lightPos);
	_shader->setFloat3("light.ambient", glm::vec3(0.4f, 0.4f, 0.4f));
	_shader->setFloat3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));

	// skybox
	auto skybox = glm::mat4(1.0f);
	skybox = translate(skybox, glm::vec3(_skybox->position.x, _skybox->position.y, _skybox->position.z));
	skybox = scale(skybox, glm::vec3(_skybox->scale.x, _skybox->scale.y, _skybox->scale.z));
	skybox = rotate(skybox, _skybox->rotation += deltaTime / 8, glm::vec3(0.0f, 1.0f, 0.0f));

	_shader->setFloatMat4("model", skybox);
	_skybox->Draw(*_shader);

	// player
	auto player = glm::mat4(1.0f);
	player = translate(player, glm::vec3(_player->position.x, _player->position.y, _player->position.z));
	player = scale(player, glm::vec3(_player->scale.x, _player->scale.y, _player->scale.z));

	_shader->setFloatMat4("model", player);
	_player->Draw(*_shader);

	// ball
	auto ball = glm::mat4(1.0f);
	ball = translate(ball, glm::vec3(_ball->position.x, _ball->position.y, _ball->position.z));
	ball = scale(ball, glm::vec3(_ball->scale.x, _ball->scale.y, _ball->scale.z));

	_shader->setFloatMat4("model", ball);
	_ball->Draw(*_shader);

	// level - 5x10 bricks for the player to destroy
	for (int y = 0; y < numbBricksHigh; y++)
	{
		for (int x = 0; x < numbBricksWide; x++)
		{
			auto brick = glm::mat4(1.0f);
			brick = translate(brick, bricks[y][x]->position);
			brick = scale(brick, bricks[y][x]->scale);
			brick = rotate(brick, bricks[y][x]->rotation += deltaTime, glm::vec3(0.0f, 1.0f, 0.0f));

			_shader->setFloatMat4("model", brick);
			bricks[y][x]->Draw(*_shader);
		}
	}

	// bounds
	// left bound
	for (int i = 0; i < boundBlocks; i++)
	{
		auto brickLeft = glm::mat4(1.0f);
		brickLeft = translate(brickLeft, boundLeft[i]->position);
		brickLeft = scale(brickLeft, boundLeft[i]->scale);
		brickLeft = rotate(brickLeft, boundLeft[i]->rotation, glm::vec3(0.0f, 1.0f, 0.0f));

		_shader->setFloatMat4("model", brickLeft);
		boundLeft[i]->Draw(*_shader);
	}

	// right bound
	for (int i = 0; i < boundBlocks; i++)
	{
		auto brickRight = glm::mat4(1.0f);
		brickRight = translate(brickRight, boundRight[i]->position);
		brickRight = scale(brickRight, boundRight[i]->scale);
		brickRight = rotate(brickRight, boundRight[i]->rotation, glm::vec3(0.0f, 1.0f, 0.0f));

		_shader->setFloatMat4("model", brickRight);
		boundRight[i]->Draw(*_shader);
	}

	// top bound
	for (int i = 0; i < topBlocks; i++)
	{
		auto brickTop = glm::mat4(1.0f);
		brickTop = translate(brickTop, boundTop[i]->position);
		brickTop = scale(brickTop, boundTop[i]->scale);
		brickTop = rotate(brickTop, boundTop[i]->rotation, glm::vec3(0.0f, 1.0f, 0.0f));

		_shader->setFloatMat4("model", brickTop);
		boundTop[i]->Draw(*_shader);
	}

	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Application::BuildLevel()
{
	for (int y = 0; y < numbBricksHigh; y++)
	{
		for (int x = 0; x < numbBricksWide; x++)
		{
			_brick = std::make_unique<Brick>("res\\models\\brick\\brick.obj");

			_brick->scale = glm::vec3(0.5f, 0.5f, 0.5f);
			_brick->position = (glm::vec3(-9.0f + (2.0f * x), (2.0f * y), 0.0f));

			bricks[y][x] = std::move(_brick);
		}
	}

	// left bound
	for (int i = 0; i < boundBlocks; i++)
	{
		_brickLeft = std::make_unique<Brick>("res\\models\\brick\\brick.obj");

		_brickLeft->scale = glm::vec3(1.0f, 1.0f, 1.0f);
		_brickLeft->position = (glm::vec3(-12.0f, -10.0f + i, 0.0f));

		boundLeft[i] = std::move(_brickLeft);
	}

	// right bound
	for (int i = 0; i < boundBlocks; i++)
	{
		_brickRight = std::make_unique<Brick>("res\\models\\brick\\brick.obj");

		_brickRight->scale = glm::vec3(1.0f, 1.0f, 1.0f);
		_brickRight->position = (glm::vec3(12.0f, -10.0f + i, 0.0f));

		boundRight[i] = std::move(_brickRight);
	}

	// top bound
	for (int i = 0; i < topBlocks; i++)
	{
		_brickTop = std::make_unique<Brick>("res\\models\\brick\\brick.obj");

		_brickTop->scale = glm::vec3(1.0f, 1.0f, 1.0f);
		_brickTop->position = (glm::vec3(-12.0f + i, 10.0f, 0.0f));
		
		boundTop[i] = std::move(_brickTop);
	}
}

void Application::UpdateCameraView(unsigned view)
{
	switch (view)
	{
	case 1:
		freeCamera = false;

		camera.Position = firstCamView;
		camera.Pitch = 45.0f;
		camera.Yaw = -90.0f;
		break;
	case 2:
		freeCamera = false;

		camera.Position = secondCamView;
		camera.Pitch = -30.0f;
		camera.Yaw = -90.0f;
		break;
	case 3:
		freeCamera = false;

		camera.Position = thirdCamView;
		camera.Pitch = 0.0f;
		camera.Yaw = -135.0f;
		break;
	case 4:
		freeCamera = true;

		camera.Position = fourthCamView;
		camera.Pitch = 0.0f;
		camera.Yaw = -90.0f;
		break;
	}

	updateView = false;
}

bool Application::IsTheGameWon()
{
	for (int y = 0; y < numbBricksHigh; y++)
	{
		for (int x = 0; x < numbBricksWide; x++)
		{
			if (bricks[y][x]->brickAlive)
			{
				return !bricks[y][x]->brickAlive;
			}
		}
	}

	return true;
}

void Application::UpdatePlayerPosition(GLFWwindow* window)
{
	// release the ball
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		stuckToPaddle = false;
	}

	if (_player->lives > 0)
	{
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		{
			if (_player->position.x > -11.25f + offset)
			{
				_player->position.x -= playerVelocity.x * deltaTime;

				auto player = glm::mat4(1.0f);
				player = translate(player, glm::vec3(_player->position.x, _player->position.y, _player->position.z));

				_shader->setFloatMat4("model", player);
				_player->Draw(*_shader);
			}
		}
		
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		{
			if (_player->position.x < 11.15f - offset)
			{
				_player->position.x += playerVelocity.x * deltaTime;

				auto player = glm::mat4(1.0f);
				player = translate(player, glm::vec3(_player->position.x, _player->position.y, _player->position.z));

				_shader->setFloatMat4("model", player);
				_player->Draw(*_shader);
			}
		}

		if (stuckToPaddle)
		{
			_ball->position = glm::vec3
			(
				_player->position.x,
				_player->position.y + _player->scale.y + (_ball->scale.y * 2),
				_player->position.z
			);

			auto ball = glm::mat4(1.0f);
			ball = translate(ball, glm::vec3(_ball->position.x, _ball->position.y, _ball->position.z));

			_shader->setFloatMat4("model", ball);
			_ball->Draw(*_shader);
		}

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	else if (_player->lives <= 0)
	{
		state = GameState::Lose;
	}
}

// TODO: Refactor this fat method
void Application::UpdateBallPosition(GLFWwindow* window)
{
	if (!stuckToPaddle)
	{
		_ball->position.x += ballVelocity.x * deltaTime;

		auto ball = glm::mat4(1.0f);
		ball = translate(ball, glm::vec3(_ball->position.x, _ball->position.y, _ball->position.z));

		_shader->setFloatMat4("model", ball);
		_ball->Draw(*_shader);
		
		// keep the ball inside the bounds of the screen
		if (_ball->position.x <= -11.0f)
		{
			ballVelocity.x = -ballVelocity.x;
			_ball->position.x = -11.0f;

			ball = translate(ball, glm::vec3(_ball->position.x, _ball->position.y, _ball->position.z));
			
			_shader->setFloatMat4("model", ball);
			_ball->Draw(*_shader);
		}
		else if (_ball->position.x >= 11.0f)
		{
			ballVelocity.x = -ballVelocity.x;
			_ball->position.x = 11.0f;

			ball = translate(ball, glm::vec3(_ball->position.x, _ball->position.y, _ball->position.z));

			_shader->setFloatMat4("model", ball);
			_ball->Draw(*_shader);
		}

		// check each brick for collision on the left and right
		for (int y = 0; y < numbBricksHigh; y++)
		{
			for (int x = 0; x < numbBricksWide; x++)
			{
				if (bricks[y][x]->brickAlive)
				{
					if (CollisionDetection(_ball, bricks[y][x]))
					{
						SetCrackedBrick(x, y);
						
						ballVelocity.x = -ballVelocity.x;
						_ball->position.x += ballVelocity.x * deltaTime;

						ball = glm::mat4(1.0f);
						ball = translate(
							ball, glm::vec3(_ball->position.x, _ball->position.y, _ball->position.z));

						_shader->setFloatMat4("model", ball);
						_ball->Draw(*_shader);
					}
					
					if (bricks[y][x]->hits < 0)
					{
						SetDeadBrick(x, y);
					}
				}
				
				// brick is dying
				if (bricks[y][x]->brickDying)
				{
					SetDyingBrick(x, y);

					auto brick = glm::mat4(1.0f);
					brick = translate(brick, glm::vec3(bricks[y][x]->position.x, bricks[y][x]->position.y, bricks[y][x]->position.z));

					_shader->setFloatMat4("model", brick);
					bricks[y][x]->Draw(*_shader);
				}

				// brick is gone
				if (bricks[y][x]->position.y < -15.0f)
				{
					bricks[y][x]->brickDying = false;
				}
			}
		}

		_ball->position.y += ballVelocity.y * deltaTime;

		ball = translate(ball, glm::vec3(_ball->position.x, _ball->position.y, _ball->position.z));

		_shader->setFloatMat4("model", ball);
		_ball->Draw(*_shader);

		// check for top side
		if (_ball->position.y >= 9.0f)
		{
			ballVelocity.y = -ballVelocity.y;
			_ball->position.y = 9.0f;

			ball = translate(ball, glm::vec3(_ball->position.x, _ball->position.y, _ball->position.z));

			_shader->setFloatMat4("model", ball);
			_ball->Draw(*_shader);
		}
		// check for bottom side
		else if (_ball->position.y <= -15.0f)
		{
			_player->lives--;
			stuckToPaddle = true;
		}
		// player side - avoid ball sticking in paddle
		else if (CollisionDetection(_ball, _player))
		{
			ballVelocity.y = 1 * abs(ballVelocity.y);
		}

		// check each brick for collision on the top and bottom
		for (int y = 0; y < numbBricksHigh; y++)
		{
			for (int x = 0; x < numbBricksWide; x++)
			{
				if (bricks[y][x]->brickAlive)
				{
					if (CollisionDetection(_ball, bricks[y][x]))
					{
						SetCrackedBrick(x, y);
						
						ballVelocity.y = -ballVelocity.y;
						_ball->position.y += ballVelocity.y * deltaTime;

						ball = translate(ball, glm::vec3(_ball->position.x, _ball->position.y, _ball->position.z));

						_shader->setFloatMat4("model", ball);
						_ball->Draw(*_shader);
					}
					
					if (bricks[y][x]->hits < 0)
					{
						SetDeadBrick(x, y);
					}
				}
				
				// brick is dying
				if (bricks[y][x]->brickDying)
				{
					SetDyingBrick(x, y);

					auto brick = glm::mat4(1.0f);
					brick = translate(
						brick, glm::vec3(bricks[y][x]->position.x, bricks[y][x]->position.y, bricks[y][x]->position.z));

					_shader->setFloatMat4("model", brick);
					bricks[y][x]->Draw(*_shader);
				}

				// brick is gone
				if (bricks[y][x]->position.y < -15.0f)
				{
					bricks[y][x]->brickDying = false;
				}
			}
		}

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

bool Application::CollisionDetection(std::unique_ptr<Ball>& ball, std::unique_ptr<Brick>& brick)
{
	// calculate the sides of ball
	float ballLeft = ball->position.x - ball->scale.x;
	float ballRight = ball->position.x + ball->scale.x;
	float ballTop = ball->position.y - ball->scale.x;
	float ballBottom = ball->position.y + ball->scale.x;

	// calculate the sides of brick
	float brickLeft = brick->position.x - brick->scale.x;
	float brickRight = brick->position.x + brick->scale.x;
	float brickTop = brick->position.y - brick->scale.x;
	float brickBottom = brick->position.y + brick->scale.x;

	// if any of the sides from the ball are outside of the brick
	if (ballBottom <= brickTop) return false;
	if (ballTop >= brickBottom) return false;
	if (ballRight <= brickLeft) return false;
	if (ballLeft >= brickRight) return false;

	// if any of the sides from the ball are inside of the brick
	return true;
}

bool Application::CollisionDetection(std::unique_ptr<Ball>& ball, std::unique_ptr<Player>& player)
{
	// calculate the sides of ball
	float ballLeft = ball->position.x - ball->scale.x;
	float ballRight = ball->position.x + ball->scale.x;
	float ballTop = ball->position.y - ball->scale.x;
	float ballBottom = ball->position.y + ball->scale.x;

	// calculate the sides of player
	float playerLeft = player->position.x - player->scale.x;
	float playerRight = player->position.x + player->scale.x;
	float playerTop = player->position.y - player->scale.y;
	float playerBottom = player->position.y + player->scale.y;

	// if any of the sides from the ball are outside of the player
	if (ballBottom <= playerTop) return false;
	if (ballTop >= playerBottom) return false;
	if (ballRight <= playerLeft) return false;
	if (ballLeft >= playerRight) return false;

	// if any of the sides from the ball are inside of the player
	return true;
}

void Application::SetCrackedBrick(const int x, const int y)
{
	bricks[y][x]->hits -= 1;
	
	score += 1;
}

void Application::SetDeadBrick(const int x, const int y)
{
	bricks[y][x]->brickDying = true;
	bricks[y][x]->brickAlive = false;
	
	score += 3;
}

void Application::SetDyingBrick(const int x, const int y)
{
	bricks[y][x]->position.y -= 9.5f * deltaTime;
	bricks[y][x]->rotation += 0.075f;
	
	if (bricks[y][x]->scale.x > 0.0f)
	{
		bricks[y][x]->scale -= 0.75f * deltaTime;
	}
}