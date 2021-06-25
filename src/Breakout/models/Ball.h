#pragma once

#include <glm/vec3.hpp>

#include "Model.h"
#include "../Texture.h"

class Ball : public Model
{
public:
	Ball();

	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 velocity;
	glm::vec3 colour;

	Texture texture;
};
