#pragma once
#include "SDL_image.h"

struct Position
{
	float posX;
	float posY;
};

struct Renderer
{
	SDL_Texture* texture;
	SDL_Rect rect;
	std::uint32_t textureWidth;
	std::uint32_t textureHeight;
};