#include "Application.h"

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

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

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    const int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

    if (state != GLFW_PRESS)
    {
        return;
    }

    const auto sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    auto* app = (Application*)glfwGetWindowUserPointer(window);

    app->_yaw += xoffset;
    app->_pitch += yoffset;

    if (app->_pitch > 89.0f)
        app->_pitch = 89.0f;

    if (app->_pitch < -89.0f)
        app->_pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(app->_yaw)) * cos(glm::radians(app->_pitch));
    direction.y = sin(glm::radians(app->_pitch));
    direction.z = sin(glm::radians(app->_yaw)) * cos(glm::radians(app->_pitch));

    app->_cameraFront = glm::normalize(direction);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    auto* app = (Application*)glfwGetWindowUserPointer(window);

    app->_fov -= (float)yoffset;

    if (app->_fov < 1.0f)
    {
        app->_fov = 1.0f;
    }

    if (app->_fov > 45.0f)
    {
        app->_fov = 45.0f;
    }
}

void Application::ProcessInput(GLFWwindow* window, float dt)
{
    const auto cameraSpeed = 2.5f * dt;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        _cameraPos += cameraSpeed * _cameraFront;
    }
	
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        _cameraPos -= cameraSpeed * _cameraFront;
    }
	
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        _cameraPos -= glm::normalize(glm::cross(_cameraFront, _cameraUp)) * cameraSpeed;
    }
	
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        _cameraPos += glm::normalize(glm::cross(_cameraFront, _cameraUp)) * cameraSpeed;
    }
}

void Application::Run()
{
	/* Initialize the glfw */
    if (!glfwInit())
        return;

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Breakout", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        return;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    glViewport(0, 0, 1280, 720);

    glfwSetWindowUserPointer(window, this);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    Init();

    float deltaTime = 0.0f;	// Time between current frame and last frame
    float lastFrame = 0.0f; // Time of last frame

    while (!glfwWindowShouldClose(window))
    {
	    const float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        ProcessInput(window, deltaTime);
        Update(deltaTime);
        Render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}

void Application::Init()
{
    glEnable(GL_DEPTH_TEST);
}

void Application::Update(float dt)
{
	
}

void Application::Render()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
