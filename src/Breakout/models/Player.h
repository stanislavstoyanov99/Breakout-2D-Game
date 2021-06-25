#pragma once

#include <glad/glad.h>
#include <glm/vec3.hpp>

#include "Model.h"
#include "../Texture.h"

class Player : public Model
{
public:
	Player();
	Player(std::string const& path, bool gamma = false);

	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 colour;

	Texture texture;

	float rotation;
	
	int lives;
	int score;
};
