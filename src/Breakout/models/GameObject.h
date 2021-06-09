#pragma once

#include "Model.h"

class GameObject : public Model
{
public:
	GameObject();
	GameObject(std::string const& path, bool gamma = false);

	glm::vec3 position;
	glm::vec3 scale;

	float rotation;

	//Is the gamobject active
	bool active;
};