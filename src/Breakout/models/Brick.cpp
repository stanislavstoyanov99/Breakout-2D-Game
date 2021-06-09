#include "Brick.h"

Brick::Brick()
	: Model()
{
	
}

Brick::Brick(std::string const& path, bool gamma)
	: Model(path, gamma)
{
	position = { 0.0f, 0.0f, 0.0f };
	scale = { 1.0f, 1.0f, 0.0f };
	
	rotation = 0.0f;
	
	hits = 1;
	
	brickAlive = true;
	brickDying = false;
}