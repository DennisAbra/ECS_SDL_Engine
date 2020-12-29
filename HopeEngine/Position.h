#pragma once
#include "SDL_image.h"
#include "Vector2.h"

struct Position
{
	Vector2 location;
};

struct Renderer
{
	SDL_Color color;
	float size;
};