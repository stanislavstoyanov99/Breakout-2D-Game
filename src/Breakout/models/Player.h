#pragma once
#include <glad/glad.h>
#include <glm/vec3.hpp>

#include "Model.h"

class Player : public Model
{
public:
	Player();
	Player(std::string const& path, bool gamma = false);

	glm::vec3 position;
	glm::vec3 scale;

	float rotation;
	
	int lives;
	int score;
};
