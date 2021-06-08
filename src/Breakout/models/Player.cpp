#include "Player.h"

Player::Player()
	: Model("", false)
{

}

Player::Player(std::string const& path, bool gamma)
	: Model(path, gamma)
{
	position = { 0.0f, 0.0f, 0.0f };
	scale = { 1.0f, 1.0f, 0.0f };
	
	rotation = 0.0f;
	
	lives = 3;
	score = 0;
}