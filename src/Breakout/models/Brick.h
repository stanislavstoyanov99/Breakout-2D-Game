#pragma once

#include <glm/vec3.hpp>

#include "Model.h"

class Brick : public Model
{
public:
	Brick();
	Brick(std::string const& path, bool gamma = false);
	
	glm::vec3 position;
	glm::vec3 scale;
	float rotation;

	unsigned hits;
	
	bool brickAlive;
	bool brickDying;
};
