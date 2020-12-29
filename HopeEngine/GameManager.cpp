#include "GameManager.h"
#include "Coordinator.h"
#include "Position.h"
#include "CubeRenderSystem.h"

Coordinator coordinator;

void GameManager::Run()
{
	coordinator.Init();
	timer = std::make_unique<Timer>();

	coordinator.RegisterComponent<Position>();
	coordinator.RegisterComponent<Renderer>();

	auto cubeSystem = coordinator.RegisterSystem<CubeRenderSystem>();

	ComponentList compList;
	compList.set(coordinator.GetComponentType<Position>());
	compList.set(coordinator.GetComponentType<Renderer>());
	coordinator.SetSystemComponentList<CubeRenderSystem>(compList);
	std::array<Entity, MAX_ENTITIES> ents;

	for (int i = 0; i < MAX_ENTITIES; i++)
	{
		Entity ent = coordinator.CreateEntity();
		SDL_Color entColor;
		entColor.b = 255.0f;
		coordinator.AddComponent(ent, Position{ Vector2(10, i*10) });
		coordinator.AddComponent(ent, Renderer{ entColor, 5.0f });
		ents[i] = ent;
	}

	
	while (!bQuit)
	{
		timer->Update();
		while (SDL_PollEvent(&events))
		{
			if (events.type == SDL_QUIT)
				bQuit = true;
		}
		timer->Reset();

		PreUpdate();
		Update();
		cubeSystem->Update(timer->DeltaTime());
		LateUpdate();

		Render();
	}
}

void GameManager::Update()
{


}

void GameManager::LateUpdate()
{

}

void GameManager::PreUpdate()
{
	coordinator.RenderClearBackBuffer();
}

void GameManager::Render()
{
	time += timer->DeltaTime();
	frames++;
	if (time >= 1)
	{
		frames = 0;
		time = 0;
	}

	coordinator.Render();
}
