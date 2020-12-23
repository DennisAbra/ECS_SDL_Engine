#include "GameManager.h"


void GameManager::Setup()
{
	windowResource = std::make_unique<WindowResource>();
	timer = std::make_unique<Timer>();
	componentManager = std::make_unique<ComponentManager>();
	systemManager = std::make_unique<SystemManager>();
	entityManager = std::make_unique<EntityManager>();
}

void GameManager::UpdateLoop()
{
	while (!bQuit)
	{
		//Update timer
		while (SDL_PollEvent(&events))
		{
			if (events.type == SDL_QUIT)
				bQuit = true;
		}

		PreUpdate();
		EarlyUpdate();
		Update();
		LateUpdate();

		Render();
	}
}

void GameManager::EarlyUpdate()
{

}

void GameManager::Update()
{
	SDL_Color color;
	color.a = 255;
	color.r = 255;
	windowResource->DrawSquare(Vector2(50.0f, 50.0f), 5.0f, color);
	
}

void GameManager::LateUpdate()
{

}

void GameManager::PreUpdate()
{
}

void GameManager::Render()
{
	time += timer->DeltaTime();
	frames++;
	if (time >= 1)
	{
		windowResource->PrintBorderFrameCounter(frames);
		frames = 0;
		time = 0;
	}

	windowResource->Render();
}
