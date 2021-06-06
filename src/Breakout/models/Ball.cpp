#include "Ball.h"

//Constructor
Ball::Ball()
{
	_position = { 0.0f, 0.0f, 0.0f };
	_scale = { 1.0f, 1.0f, 0.0f };
	_velocity = { 0.0f, 0.0f, 0.0f };
	_color = { 1.0f, 1.0f, 1.0f };
	
	_radius = 1.0f;
}