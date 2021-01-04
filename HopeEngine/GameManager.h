#pragma once
#include "SDL_events.h"
#include "WindowResource.h"
#include <memory>
#include "Timer.h"



class GameManager
{
public:

	void Run();


	void Update();
	void LateUpdate();

private:
	SDL_Event events;
	bool bQuit = false;

	int frames = 0;
	float time = 0;

	void RegisterComponents();
	void PreUpdate();
	void Render();

	std::unique_ptr<Timer> timer;
};