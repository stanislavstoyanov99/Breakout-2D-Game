#include "GameObject.h"

GameObject::GameObject()
	: Model()
{

}

GameObject::GameObject(std::string const& path, bool gamma)
	: Model(path, gamma)
{
	//Defualt values
	position = { 0.0f, 0.0f, 0.0f };
	scale = { 1.0f, 1.0f, 0.0f };
	rotation = { 0.0f };
	active = true;
}