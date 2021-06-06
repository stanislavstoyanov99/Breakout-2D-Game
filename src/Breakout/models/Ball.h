#pragma once

#include <glad/glad.h>
#include <glm/vec3.hpp>

class Ball
{
public:
	//Constructor
	Ball();

private:
	//Variables
	glm::vec3 _position;
	glm::vec3 _scale;
	glm::vec3 _velocity;
	glm::vec3 _color;

	GLfloat _radius;
};
