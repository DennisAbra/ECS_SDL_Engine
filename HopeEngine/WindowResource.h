#pragma once
#include <SDL_image.h>
#include <SDL.h>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <cassert>
#include "Vector2.h"

class WindowResource
{
public:
	WindowResource()
	{
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
		{
			printf("SDL Initialization Error: %s\n", SDL_GetError());
		}

		window = SDL_CreateWindow(windowName, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		assert(window != nullptr && "Window Creation error %s\n", SDL_GetError());

		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		assert(renderer != nullptr && "Render Creation error %s\n", SDL_GetError());

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);


		int flags = IMG_INIT_PNG;
		if (!(IMG_Init(flags) & flags))
		{
			printf("Error IMG initialization %s\n", IMG_GetError());
		}

		backBuffer = SDL_GetWindowSurface(window);
	}
	~WindowResource()
	{
		SDL_DestroyWindow(window);
	}
	SDL_Window* GetWindow()
	{
		return window;
	}

	SDL_Renderer* GetRenderer()
	{
		return renderer;
	}
	void ClearBackBuffer()
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);
	}

	void Render()
	{
		SDL_RenderPresent(renderer);
	}

	void PrintBorderFrameCounter(int frames)
	{
		char buffer[64];
		sprintf_s(buffer, "%s%s%s%d", windowName, " | ", "FPS: ", frames);
		SDL_SetWindowTitle(window, buffer);
	}


	void DrawTexture(SDL_Texture* tex, SDL_Rect* clip, SDL_Rect* rect, float angle, SDL_RendererFlip flip)
	{
		//assert(tex != nullptr && "Trying to draw a null texture");
		SDL_RenderCopyEx(renderer, tex, NULL, rect, angle, NULL, flip);
	}

	/**
	*  \param position1  =  Start point
	*  \param position2  =  End Point
	*  \param color     =  Color of the line
	*/
	void DrawLine(Vector2 position1, Vector2 position2, SDL_Color color)
	{
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
		SDL_RenderDrawLine(renderer, position1.x, position1.y, position2.x, position2.y);
	}

	void DrawCircle(int32_t centerX, int32_t centerY, int32_t radius)
	{
		const int32_t diameter = radius * 2;

		int32_t x = radius - 1;
		int32_t y = 0;
		int32_t tx = 1;
		int32_t ty = 1;
		int32_t error = tx - diameter;

		while (x >= y)
		{
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 100);
			SDL_RenderDrawPoint(renderer, centerX + x, centerY - y);
			SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
			SDL_RenderDrawPoint(renderer, centerX - x, centerY - y);
			SDL_RenderDrawPoint(renderer, centerX - x, centerY + y);
			SDL_RenderDrawPoint(renderer, centerX + x, centerY - y);
			SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
			SDL_RenderDrawPoint(renderer, centerX - x, centerY - y);
			SDL_RenderDrawPoint(renderer, centerX - x, centerY + y);

			if (error <= 0)
			{
				++y; 
				error += ty;
				ty += 2;
			}

			if (error > 0)
			{
				--x;
				tx += 2;
				error += tx - diameter;
			}
		}
	}


	/**
	*  \param position  =  The center position of the square
	*  \param size      =  The Radius of the square
	*  \param color     =  The Border color of the square	*
	*/
	void DrawSquare(Vector2 position, float size, SDL_Color color)
	{
		SDL_Rect rect = { position.x + size,  position.y - size, size * 2, size * 2 };
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
		SDL_RenderDrawRect(renderer, &rect);
	}

	/**
	*  \param position  =  The center position of the square
	*  \param size      =  The Radius of the square
	*  \param color     =  The Color of the square	*
	*/
	void DrawFilledSquare(Vector2 position, float size, SDL_Color color)
	{
		SDL_Rect rect = { position.x + size,  position.y - size, size * 2, size * 2 };
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
		SDL_RenderFillRect(renderer, &rect);
	}




	// Loads Texture from asset in designated asset folder with filename. 
	// Also saves texture with given name.
	void LoadNewTexture(std::string fileName, const char* textureName)
	{
		Texturemap[textureName] = LoadTexture(fileName);
	}

	SDL_Texture* GetTexture(const char* textureName)
	{
		assert(Texturemap.find(textureName) != Texturemap.end());
		return Texturemap[textureName];
	}



	const char* windowName = "Hope";
	const std::string graphicsFolderPath = "Assets/";
private:

	const std::uint32_t SCREEN_WIDTH = 1280;
	const std::uint32_t SCREEN_HEIGHT = 720;

	SDL_Renderer* renderer;
	SDL_Window* window;
	SDL_Surface* backBuffer;
	std::unordered_map<const char*, SDL_Texture*> Texturemap;

	SDL_Texture* LoadTexture(std::string path)
	{
		SDL_Texture* tex = nullptr;
		std::string fullpath = SDL_GetBasePath();
		fullpath.append(graphicsFolderPath + path);
		SDL_Surface* surface = IMG_Load(fullpath.c_str());

		if (surface == nullptr)
		{
			printf("IMG Load failed when trying to load texture %s\n", IMG_GetError());
			return tex;
		}

		tex = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);
		return tex;
	}


};