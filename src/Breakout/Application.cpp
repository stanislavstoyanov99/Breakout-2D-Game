#include "Application.h"

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Settings.h"
#include "Shader.h"

#include "models/Ball.h"
#include "models/Brick.h"
#include "models/Player.h"
#include "models/GameObject.h"
#include "models/Sprite.h"

// game states
enum class GameState { Play, Win, Lose, Exit };
GameState state;

// camera initialization
Camera camera(glm::vec3(0.0f, 0.0f, 28.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

// camera views
glm::vec3 firstCamView = glm::vec3(0.0f, -20.0f, 15.0f);
glm::vec3 secondCamView = glm::vec3(0.0f, 15.0f, 20.0f);
glm::vec3 thirdCamView = glm::vec3(25.0f, 0.0f, 20.0f);;
glm::vec3 fourthCamView = camera.Position;

// camera variables
bool updateView;
bool freeCamera;

// screen dimensions
int screenWidth = SCR_WIDTH;
int screenHeight = SCR_HEIGHT;

glm::vec3 ballVelocity = { 5.5f, 10.0f, 0.0f };
bool stuckToPaddle;
float offset = 0;

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

// game objects
std::vector<std::unique_ptr<Texture>> scoreText;
std::vector<std::unique_ptr<Sprite>> scoreObject;

// matrices
glm::mat4 orthoProgMatrix;
glm::mat4 orthoViewMatrix;
glm::mat4 modelTranslate;
glm::mat4 modelScale;
glm::mat4 modelRotation;

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
	}
	else
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

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
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	state = GameState::Play;
	gameWon = false;

	updateView = false;
	freeCamera = false;

	// ball stuck to paddle
	stuckToPaddle = true;

	// player score
	score = 0;

	_shader = std::make_unique<Shader>("res\\projection.vert.glsl", "res\\projection.frag.glsl");
	_spriteShader = std::make_unique<Shader>("res\\spriteProjection.vert.glsl", "res\\spriteProjection.frag.glsl");
	
	// load background model
	{
		_background = std::make_unique<GameObject>();
		_background->loadASSIMP("res\\models\\background\\cube.obj");
		_background->setBuffers();

		_background->texture.Load("res\\content\\skycube.png");
		
		_background->position = glm::vec3(0.0f);
		_background->scale = glm::vec3(100.0f);
	}

	// load player model
	{
		_player = std::make_unique<Player>();
		_player->loadASSIMP("res\\models\\player\\cube.obj");
		_player->setBuffers();

		_player->position = glm::vec3(0.0f, -9.5f, 0.0f);
		_player->scale = glm::vec3(1.5f, 0.125f, 0.5f);
		_player->colour = { 1.0f, 0.0f, 0.0f };
		
		offset = _player->scale.x;

		_player->texture.Load("res\\content\\player.png");
	}

	// load ball model
	{
		_ball = std::make_unique<Ball>();
		_ball->loadASSIMP("res\\models\\ball\\sphere.obj");
		_ball->setBuffers();

		_ball->position = glm::vec3
		(
			_player->position.x,
			_player->position.y + _ball->scale.y,
			_player->position.z
		);
		
		_ball->scale = glm::vec3(0.1f, 0.1f, 0.1f);

		_ball->texture.Load("res\\content\\newball.png");
	}

	// level
	{
		BuildLevel();
	}

	// lives
	{
		_lives = std::make_unique<Sprite>();
		_lives->SetBuffers();
		
		_lives->scale = glm::vec3(30.0f, 30.0f, 1.0f);
		_lives->position = glm::vec3
		(
			30.0f,
			(GLfloat)screenHeight - _lives->scale.y - 5.0f,
			0.0f
		);

		_lives->texture.Load("res\\content\\heart.png");
	}

	// win
	{
		_win = std::make_unique<Sprite>();
		_win->SetBuffers();
		
		_win->scale = glm::vec3(screenWidth / 2, screenHeight / 2, 1.0f);
		_win->position = glm::vec3
		(
			screenWidth / 4,
			screenHeight / 4,
			0.0f
		);
		
		_win->texture.Load("res\\content\\youWin.png");
		_win->active = false;
	}
	
	// gameover
	{
		_gameover = std::make_unique<Sprite>();
		_gameover->SetBuffers();
		
		_gameover->scale = glm::vec3(screenWidth / 2, screenHeight / 2, 1.0f);
		_gameover->position = glm::vec3
		(
			screenWidth / 4,
			screenHeight / 4,
			0.0f
		);
		
		_gameover->texture.Load("res\\content\\gameOver.png");
		_gameover->active = false;
	}

	LoadScore();

	glEnable(GL_DEPTH_TEST);
}

void Application::Update(GLFWwindow* window, float dt)
{
	orthoViewMatrix = glm::mat4(1.0f);
	
	orthoProgMatrix = glm::ortho(0.0f, (float)SCR_WIDTH, (float)SCR_HEIGHT, 0.0f, -1.0f, 1.0f);

	glDisable(GL_DEPTH_TEST);
	
	_spriteShader->use();
	_spriteShader->setFloatMat4("uView", orthoViewMatrix);
	_spriteShader->setFloatMat4("uProjection", orthoProgMatrix);
	_spriteShader->unuse();
	
	if (state == GameState::Play)
	{
		gameWon = IsTheGameWon();
		
		if (gameWon)
		{
			state = GameState::Win;
		}

		UpdatePlayerPosition(window);

		UpdateBallPosition(window);
	}
	else if (state == GameState::Win)
	{
		_win->active = true;
	}
	else if (state == GameState::Lose)
	{
		_gameover->active = true;
	}
}

void Application::Render()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_shader->use();
	
	// move light source
	glm::mat4 rotationMat(1);
	rotationMat = glm::rotate(rotationMat, _lightRotation, glm::vec3(1.0f, 1.0f, 0.0f));
	_lightPos = glm::vec3(rotationMat * glm::vec4(_lightPos, 1.0));
	
	// background
	ResetMatrices();
	modelTranslate = translate(modelTranslate, glm::vec3(_background->position.x, _background->position.y, _background->position.z));
	modelScale = scale(modelScale, glm::vec3(_background->scale.x, _background->scale.y, _background->scale.z));
	modelRotation = rotate(modelRotation, _background->rotation += deltaTime / 8, glm::vec3(0.0f, 1.0f, 0.0f));

	RenderObject(_shader, modelTranslate, modelScale, modelRotation, _background->colour, _background->texture);
	_background->render();
	
	// player
	ResetMatrices();
	modelTranslate = translate(modelTranslate, glm::vec3(_player->position.x, _player->position.y, _player->position.z));
	modelScale = scale(modelScale, glm::vec3(_player->scale.x, _player->scale.y, _player->scale.z));
	modelRotation = glm::rotate(modelRotation, _player->rotation, glm::vec3(0.0f, 1.0f, 0.0f));

	RenderObject(_shader, modelTranslate, modelRotation, modelScale, _player->colour, _player->texture);
	_player->render();
	
	// ball
	ResetMatrices();
	modelTranslate = translate(modelTranslate, _ball->position);
	modelRotation = glm::rotate(modelRotation, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	modelScale = scale(modelScale, _ball->scale);

	RenderObject(_shader, modelTranslate, modelRotation, modelScale, _ball->colour, _ball->texture);
	_ball->render();
	
	// level - 5x10 bricks for the player to destroy
	for (int y = 0; y < numbBricksHigh; y++)
	{
		for (int x = 0; x < numbBricksWide; x++)
		{
			ResetMatrices();
			modelTranslate = translate(modelTranslate, bricks[y][x]->position);
			modelScale = scale(modelScale, bricks[y][x]->scale);
			modelRotation = rotate(modelRotation, bricks[y][x]->rotation += deltaTime, glm::vec3(0.0f, 1.0f, 0.0f));
			
			RenderObject(_shader, modelTranslate, modelRotation, modelScale, bricks[y][x]->colour, bricks[y][x]->texture);
			bricks[y][x]->render();
		}
	}

	// bounds
	// left bound
	for (int i = 0; i < boundBlocks; i++)
	{
		ResetMatrices();
		modelTranslate = translate(modelTranslate, boundLeft[i]->position);
		modelScale = scale(modelScale, boundLeft[i]->scale);
		modelRotation = rotate(modelRotation, boundLeft[i]->rotation, glm::vec3(0.0f, 1.0f, 0.0f));

		RenderObject(_shader, modelTranslate, modelRotation, modelScale, boundLeft[i]->colour, boundLeft[i]->texture);
		boundLeft[i]->render();
	}

	// right bound
	for (int i = 0; i < boundBlocks; i++)
	{
		ResetMatrices();
		modelTranslate = translate(modelTranslate, boundRight[i]->position);
		modelScale = scale(modelScale, boundRight[i]->scale);
		modelRotation = rotate(modelRotation, boundRight[i]->rotation, glm::vec3(0.0f, 1.0f, 0.0f));

		RenderObject(_shader, modelTranslate, modelRotation, modelScale, boundRight[i]->colour, boundRight[i]->texture);
		boundRight[i]->render();
	}

	// top bound
	for (int i = 0; i < topBlocks; i++)
	{
		ResetMatrices();
		modelTranslate = translate(modelTranslate, boundTop[i]->position);
		modelScale = scale(modelScale, boundTop[i]->scale);
		modelRotation = rotate(modelRotation, boundTop[i]->rotation, glm::vec3(0.0f, 1.0f, 0.0f));

		RenderObject(_shader, modelTranslate, modelRotation, modelScale, boundTop[i]->colour, boundTop[i]->texture);
		boundTop[i]->render();
	}

	_shader->unuse();
	
	glDisable(GL_DEPTH_TEST);
	
	_spriteShader->use();
	
	// lives
	{
		for (int i = 0; i < _player->lives; i++)
		{
			ResetMatrices();
			
			modelTranslate = glm::translate(modelTranslate, glm::vec3(_lives->position.x + (i * 40.0f), _lives->position.y, _lives->position.z));
			modelScale = glm::scale(modelScale, _lives->scale);

			RenderSprite(_spriteShader, modelTranslate, modelScale, _lives->colour, _lives->texture);
			_lives->Render();
		}
	}

	// win/gameover textures
	{
		if (_win->active)
		{
			ResetMatrices();
			
			modelTranslate = glm::translate(modelTranslate, _win->position);
			modelScale = glm::scale(modelScale, _win->scale);

			RenderSprite(_spriteShader, modelTranslate, modelScale, _win->colour, _win->texture);
			_win->Render();
		}

		if (_gameover->active)
		{
			ResetMatrices();
			
			modelTranslate = glm::translate(modelTranslate, _gameover->position);
			modelScale = glm::scale(modelScale, _gameover->scale);
			
			RenderSprite(_spriteShader, modelTranslate, modelScale, _gameover->colour, _gameover->texture);
			_gameover->Render();
		}
	}

	// score
	{
		for (auto& sprite : scoreObject)
		{
			ResetMatrices();
			
			modelTranslate = glm::translate(modelTranslate, sprite->position);
			modelScale = glm::scale(modelScale, sprite->scale);

			RenderSprite(_spriteShader, modelTranslate, modelScale, sprite->colour, sprite->texture);
			sprite->Render();
		}
	}

	glDrawArrays(GL_TRIANGLES, 0, 36);

	_spriteShader->unuse();
}

void Application::BuildLevel()
{
	auto blockTexture = std::make_unique<Texture>();
	blockTexture->Load("res\\content\\block.png");
	
	auto crackedTexture = std::make_unique<Texture>();
	crackedTexture->Load("res\\content\\crackedBlock.png");
	
	for (int y = 0; y < numbBricksHigh; y++)
	{
		for (int x = 0; x < numbBricksWide; x++)
		{
			_brick = std::make_unique<Brick>();
			_brick->loadASSIMP("res\\models\\brick\\cube.obj");
			_brick->setBuffers();

			_brick->scale = glm::vec3(0.5f, 0.5f, 0.5f);
			_brick->position = (glm::vec3(-9.0f + (2.0f * x), (2.0f * y), 0.0f));

			if (y == 0 || y == 5)
			{
				_brick->colour = { 0.0f, 0.5f, 1.0f };
			}
			else if (y == 1 || y == 6)
			{
				_brick->colour = { 1.0f, 1.0f, 0.0f };
			}
			else if (y == 2 || y == 7)
			{
				_brick->colour = { 1.0f, 0.0f, 0.0f };
			}
			else if (y == 3 || y == 8)
			{
				_brick->colour = { 0.0f, 1.0f, 0.0f };
			}
			else if (y == 4 || y == 9)
			{
				_brick->colour = { 1.0f, 1.0f, 1.0f };
			}
			else
			{
				_brick->colour = { 1.0f, 1.0f, 1.0f };
			}

			_brick->texture = *std::move(blockTexture);
			_brick->cracked = *std::move(crackedTexture);

			bricks[y][x] = std::move(_brick);
		}
	}

	// left bound
	for (int i = 0; i < boundBlocks; i++)
	{
		_brickLeft = std::make_unique<Brick>();
		_brickLeft->loadASSIMP("res\\models\\brick\\cube.obj");
		_brickLeft->setBuffers();

		_brickLeft->scale = glm::vec3(0.5f, 0.5f, 0.5f);
		_brickLeft->position = (glm::vec3(-12.0f, -10.0f + i, 0.0f));
		_brickLeft->colour = { 1.0f, 1.0f, 1.0f };

		_brickLeft->texture = *std::move(blockTexture);

		boundLeft[i] = std::move(_brickLeft);
	}

	// top bound
	for (int i = 0; i < topBlocks; i++)
	{
		_brickTop = std::make_unique<Brick>();
		_brickTop->loadASSIMP("res\\models\\brick\\cube.obj");
		_brickTop->setBuffers();

		_brickTop->scale = glm::vec3(0.5f, 0.5f, 0.5f);
		_brickTop->position = (glm::vec3(-12.0f + i, 10.0f, 0.0f));
		_brickTop->colour = { 0.0f, 1.0f, 0.0f };

		_brickTop->texture = *std::move(blockTexture);

		boundTop[i] = std::move(_brickTop);
	}
	
	// right bound
	for (int i = 0; i < boundBlocks; i++)
	{
		_brickRight = std::make_unique<Brick>();
		_brickRight->loadASSIMP("res\\models\\brick\\cube.obj");
		_brickRight->setBuffers();

		_brickRight->scale = glm::vec3(0.5f, 0.5f, 0.5f);
		_brickRight->position = (glm::vec3(12.0f, -10.0f + i, 0.0f));
		_brickRight->colour = { 1.0f, 0.0f, 0.0f };

		_brickRight->texture = *std::move(blockTexture);

		boundRight[i] = std::move(_brickRight);
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
			}
		}
		
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		{
			if (_player->position.x < 11.15f - offset)
			{
				_player->position.x += playerVelocity.x * deltaTime;
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
		
		// keep the ball inside the bounds of the screen
		if (_ball->position.x <= -11.0f)
		{
			ballVelocity.x = -ballVelocity.x;
			_ball->position.x = -11.0f;
		}
		else if (_ball->position.x >= 11.0f)
		{
			ballVelocity.x = -ballVelocity.x;
			_ball->position.x = 11.0f;
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
					}
					
					if (bricks[y][x]->hits < 0)
					{
						SetDeadBrick(x, y);
					}
				}
				
				if (bricks[y][x]->brickDying)
				{
					SetDyingBrick(x, y);
				}

				if (bricks[y][x]->position.y < -15.0f)
				{
					bricks[y][x]->brickDying = false;
				}
			}
		}

		_ball->position.y += ballVelocity.y * deltaTime;

		// check for top side
		if (_ball->position.y >= 9.0f)
		{
			ballVelocity.y = -ballVelocity.y;
			_ball->position.y = 9.0f;
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
					}
					
					if (bricks[y][x]->hits < 0)
					{
						SetDeadBrick(x, y);
					}
				}
				
				if (bricks[y][x]->brickDying)
				{
					SetDyingBrick(x, y);
				}

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
	bricks[y][x]->texture = std::move(bricks[y][x]->cracked);
	
	score += 1;
	SetScore();
}

void Application::SetDeadBrick(const int x, const int y)
{
	bricks[y][x]->brickDying = true;
	bricks[y][x]->brickAlive = false;
	
	score += 3;
	SetScore();
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

void Application::RenderSprite(std::unique_ptr<Shader>& shader, glm::mat4 translation, glm::mat4 scale, glm::vec3 colour, Texture& texture)
{
	shader->setFloatMat4("uModel", glm::mat4(translation * scale));
	shader->setFloatMat4("uView", glm::mat4(orthoViewMatrix));
	shader->setFloatMat4("uProjection", glm::mat4(orthoProgMatrix));
	shader->setFloat3("uColour", glm::vec3(colour.x, colour.y, colour.z));

	glBindTexture(GL_TEXTURE_2D, texture.GetTexture());
}

void Application::RenderObject(std::unique_ptr<Shader>& shader, glm::mat4 translation, glm::mat4 rotation, glm::mat4 scale, glm::vec3 colour, Texture& texture)
{
	// lighting
	shader->setFloat3("uObjectColour", glm::vec3(colour.x, colour.y, colour.z));
	shader->setFloat3("uLightColour", glm::vec3(_lightColour.x, _lightColour.y, _lightColour.z));
	shader->setFloat3("uLightPosition", glm::vec3(_lightPos.x, _lightPos.y, _lightPos.z));
	shader->setFloat3("uViewPosition", glm::vec3(camera.Position.x, camera.Position.y, camera.Position.z));

	shader->setFloatMat4("uModel", glm::mat4(translation * rotation * scale));
	shader->setFloatMat4("uView", glm::mat4(camera.GetViewMatrix()));
	shader->setFloatMat4("uProjection", glm::mat4(glm::perspective(glm::radians(camera.Zoom), (float)screenWidth / (float)screenHeight, 0.1f, 1000.0f)));

	glBindTexture(GL_TEXTURE_2D, texture.GetTexture());
}

void Application::ResetMatrices()
{
	modelTranslate = glm::mat4(1.0f);
	modelScale = glm::mat4(1.0f);
	modelRotation = glm::mat4(1.0f);
}

void Application::LoadScore()
{
	const glm::vec3 scale = glm::vec3(40.0f, 40.0f, 1.0f);

	for (int i = 0; i < 4; i++)
	{
		auto sprite = std::make_unique<Sprite>();
		sprite->SetBuffers();

		if (i == 0)
		{
			sprite->scale = glm::vec3(160.0f, 50.0f, 1.0f);
			sprite->position = glm::vec3
			(
				(GLfloat)screenWidth - 280.0f,
				(GLfloat)screenHeight - sprite->scale.y - (-5.0f),
				0.0f
			);
			
			sprite->texture.Load("res\\content\\newScore.png");
		}
		else
		{
			sprite->scale = scale;
			sprite->position = glm::vec3
			(
				(GLfloat)screenWidth - 140.0f + (i * 30.0f),
				(GLfloat)screenHeight - sprite->scale.y - (-5.0f),
				0.0f
			);
			
			sprite->texture.Load("res\\content\\0.png");
		}
		
		scoreObject.push_back(std::move(sprite));
	}

	// Load each number texture
	for (int i = 0; i < 10; i++)
	{
		auto texture = std::make_unique<Texture>();
		auto file = "res\\content\\" + std::to_string(i) + ".png";
		
		texture->Load(file);
		
		scoreText.push_back(std::move(texture));
	}
}

void Application::SetScore()
{
	std::string scoreStr = std::to_string(score);

	int pos = static_cast<int>(scoreObject.size()) - 1;

	// Loop through the score string from the end to start
	for (int i = (int)scoreStr.length() - 1; i >= 0; i--)
	{
		int value = scoreStr[i] - '0';
		scoreObject[pos]->texture = std::move(*scoreText[value]);
		pos--;
	}
}