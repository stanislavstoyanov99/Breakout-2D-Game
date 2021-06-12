#pragma once

#include <glm/vec3.hpp>

#include "Model.h"

class Ball : public Model
{
public:
	Ball();
	Ball(std::string const& path, bool gamma = false);

	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 velocity;
};
