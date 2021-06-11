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
Camera camera(glm::vec3(0.0f, 0.0f, 25.0f));
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
bool followPlayer;

// screen dimensions
int screenWidth = SCR_WIDTH;
int screenHeight = SCR_HEIGHT;
float aspectRatio = (float)SCR_HEIGHT / (float)SCR_WIDTH;

// ball
glm::vec3 ballVelocity = {};
bool stuckToPaddle;
float offset = 0;

// player
glm::vec3 playerVelocity = {};
bool moveLeft;
bool moveRight;
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
		Update(deltaTime);

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
	followPlayer = false;

	// ball stuck to paddle
	stuckToPaddle = true;

	// player controls
	moveLeft = false;
	moveRight = false;

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
		_ball->scale = glm::vec3(0.5f, 0.5f, 0.5f);
		_ball->radius = _ball->scale.x / 2;
	}

	// level
	{
		BuildLevel();
	}

	glEnable(GL_DEPTH_TEST);
}

void Application::Update(float dt)
{
	const auto viewMatrix = camera.GetViewMatrix();

	const auto projectionMatrix =
		glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);

	_shader->use();
	_shader->setFloatMat4("view", viewMatrix);
	_shader->setFloatMat4("projection", projectionMatrix);
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

		_brickLeft->scale = glm::vec3(0.5f, 0.5f, 0.5f);
		_brickLeft->position = (glm::vec3(-12.0f, -10.0f + (1.0f * i), 0.0f));

		boundLeft[i] = std::move(_brickLeft);
	}

	// right bound
	for (int i = 0; i < boundBlocks; i++)
	{
		_brickRight = std::make_unique<Brick>("res\\models\\brick\\brick.obj");

		_brickRight->scale = glm::vec3(0.5f, 0.5f, 0.5f);
		_brickRight->position = (glm::vec3(12.0f, -10.0f + (1.0f * i), 0.0f));

		boundRight[i] = std::move(_brickRight);
	}

	// top bound
	for (int i = 0; i < topBlocks; i++)
	{
		_brickTop = std::make_unique<Brick>("res\\models\\brick\\brick.obj");

		_brickTop->scale = glm::vec3(0.5f, 0.5f, 0.5f);
		_brickTop->position = (glm::vec3(-12.0f + (1.0f * i), 10.0f, 0.0f));

		//Set the bricks
		boundTop[i] = std::move(_brickTop);
	}
}

void Application::UpdateCameraView(unsigned view)
{
	switch (view)
	{
	case 1:
		followPlayer = false;
		freeCamera = false;

		camera.Position = firstCamView;
		camera.Pitch = 45.0f;
		camera.Yaw = -90.0f;
		break;
	case 2:
		followPlayer = false;
		freeCamera = false;

		camera.Position = secondCamView;
		camera.Pitch = -30.0f;
		camera.Yaw = -90.0f;
		break;
	case 3:
		followPlayer = false;
		freeCamera = false;

		camera.Position = thirdCamView;
		camera.Pitch = 0.0f;
		camera.Yaw = -135.0f;
		break;
	case 4:
		followPlayer = false;
		freeCamera = true;

		camera.Position = fourthCamView;
		camera.Pitch = 0.0f;
		camera.Yaw = -90.0f;
		break;
	}

	updateView = false;
}