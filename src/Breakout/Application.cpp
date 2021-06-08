#include "Application.h"

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "Settings.h"
#include "Shader.h"
#include "Texture.h"
#include "models/Ball.h"
#include "models/Brick.h"
#include "models/Player.h"

// camera initialization
Camera camera(glm::vec3(0.0f, 0.0f, 25.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

// camera views
glm::vec3 ballView = camera.Position;
glm::vec3 playerView = camera.Position;
glm::vec3 firstCamView = glm::vec3(0.0f, -15.0f, 15.0f);
glm::vec3 secondCamView = glm::vec3(0.0f, 15.0f, 20.0f);
glm::vec3 thirdCamView = glm::vec3(25.0f, 0.0f, 20.0f);;
glm::vec3 fourthCamView = camera.Position;

// camera variables
bool updateView;
bool freeCamera;
bool followPlayer;

// ball
glm::vec3 ballVelocity = {};
bool stuckToPaddle;
float offset = 0;

// player
glm::vec3 playerVelocity = {};
bool moveLeft;
bool moveRight;
int score;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    static bool first_mouse = true;
    static double lastX;
    static double lastY;

    if (first_mouse)
    {
        lastX = xpos;
        lastY = ypos;
        first_mouse = false;
    }

    const float xoffset = xpos - lastX;
    const float yoffset = lastY - ypos;
	
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void Application::ProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

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

void Application::Run()
{
    // glfw: initialize and configure
    if (!glfwInit())
        return;

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, WINDOW_TITLE, NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        return;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // tell GLFW to capture our mouse
    /*glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);*/
	
    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    Init();
	
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
	    const float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

    	// handle input
        ProcessInput(window);

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
    updateView = false;
    freeCamera = false;
    followPlayer = false;
	
    // ball stuck to paddle
    stuckToPaddle = true;

    // player controls
    moveLeft = false;
    moveRight = false;
	
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    std::unique_ptr<VertexBuffer> vbo(new VertexBuffer(vertices, sizeof(vertices)));

    vbo->Bind();

    vbo->SetLayout(
        {
            {"aPos", Float3},
            {"aNorm", Float3},
            {"aTexCoords", Float2}
        }
    );

    _va = std::make_unique<VertexArray>();
    _va->SetVertexBuffer(std::move(vbo));
    _va->Bind();

    _shader = std::make_unique<Shader>("res\\lighting.vert.glsl", "res\\lighting.frag.glsl");
    _lightCubeShader = std::make_unique<Shader>("res\\light_cube.vert.glsl", "res\\light_cube.frag.glsl");

	// load player model
    {
        _player = std::make_unique<Player>("res\\models\\cube\\cube.obj");
    	
        _player->position = glm::vec3(0.0f,-7.5f,0.0f);
        _player->scale = glm::vec3(1.5f, 0.125f, 0.5f);
    	
        offset = _player->scale.x;
    }

    // load ball model
    {
        _ball = std::make_unique<Ball>("res\\models\\sphere\\sphere.obj");

        _ball->position = glm::vec3(_player->position.x, _player->position.y + _ball->scale.y,_player->position.z);
        _ball->scale = glm::vec3(0.1f, 0.1f, 0.1f);
        _ball->radius = _ball->scale.x / 2;
    }
	
    glEnable(GL_DEPTH_TEST);
}

void Application::Update(float dt)
{
    const auto viewMatrix = camera.GetViewMatrix();
	
    const auto projectionMatrix = 
        glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    _shader->use();
    _shader->setFloatMat4("view", viewMatrix);
    _shader->setFloatMat4("projection", projectionMatrix);

    _lightCubeShader->use();
    _lightCubeShader->setFloatMat4("view", viewMatrix);
    _lightCubeShader->setFloatMat4("projection", projectionMatrix);
}

void Application::Render()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _shader->use();

    _shader->setFloat3("viewPos", camera.Position);

    _shader->setFloat("material.shininess", 32.0f);

    _shader->setFloat3("light.position", _lightPos);
    _shader->setFloat3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    _shader->setFloat3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f)); // darken diffuse light a bit
    _shader->setFloat3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));

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
	
    _lightCubeShader->use();
    _va->Bind();

    auto lightingModel = glm::mat4(1.0f);
    lightingModel = glm::translate(lightingModel, _lightPos);
    lightingModel = glm::scale(lightingModel, glm::vec3(0.2f));
	
    _lightCubeShader->setFloatMat4("model", lightingModel);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}
