#include "GameManager.h"
#include "Coordinator.h"
#include "Systems.h"
#include "Components.h"

Coordinator coordinator;

void GameManager::Run()
{
	coordinator.Init();
	timer = std::make_unique<Timer>();

	//RegisterComponents
	RegisterComponents();

	coordinator.LoadNewTexture("spaceship.png", "spaceship");
	coordinator.LoadNewTexture("bullet.png", "bullet");
	
	//Register Systems
	std::shared_ptr<RenderSystem> renderSystem = coordinator.RegisterSystem<RenderSystem>();
	std::shared_ptr<PlayerInputSystem> inputSystem = coordinator.RegisterSystem<PlayerInputSystem>();
	std::shared_ptr<PlayerMovementSystem> playerMovementSystem = coordinator.RegisterSystem<PlayerMovementSystem>();
	std::shared_ptr<PlayerBulletSystem> bulletSystem = coordinator.RegisterSystem<PlayerBulletSystem>();
	std::shared_ptr<MovementSystem> movementSystem = coordinator.RegisterSystem<MovementSystem>();
	std::shared_ptr<SphereCollisionSystem> sphereCollisionSystem = coordinator.RegisterSystem<SphereCollisionSystem>();

	//Register Systems componentLists
	ComponentList compList;
	compList.set(coordinator.GetComponentType<Position>());
	compList.set(coordinator.GetComponentType<Renderer>());
	coordinator.SetSystemComponentList<RenderSystem>(compList);
	compList.set(coordinator.GetComponentType<Velocity>());
	compList.set(coordinator.GetComponentType<InputData>());
	coordinator.SetSystemComponentList<PlayerInputSystem>(compList);
	compList.set(coordinator.GetComponentType<Position>());
	coordinator.SetSystemComponentList<PlayerMovementSystem>(compList);

	compList.reset();

	compList.set(coordinator.GetComponentType<Velocity>());
	compList.set(coordinator.GetComponentType<BulletData>());
	compList.set(coordinator.GetComponentType<Position>());
	coordinator.SetSystemComponentList<PlayerBulletSystem>(compList);
	
	compList.reset();
	compList.set(coordinator.GetComponentType<Position>());
	compList.set(coordinator.GetComponentType<Velocity>());
	coordinator.SetSystemComponentList<MovementSystem>(compList);

	compList.set(coordinator.GetComponentType<SphereCollider>());
	coordinator.SetSystemComponentList<SphereCollisionSystem>(compList);

	
	//Create entities

	Entity player = coordinator.CreateEntity();
	coordinator.AddComponent(player, Position{ Vector2(450.0f, 300.0f) });
	SDL_Rect r;
	r.w = 64;
	r.h = 64;
	coordinator.AddComponent(player, Renderer{ "spaceship", r });
	coordinator.AddComponent(player, Velocity{Vector2(), 1000.0f });
	coordinator.AddComponent(player, PlayerTag{});
	coordinator.AddComponent(player, InputData{});
	CollisionSet playerSet;
	playerSet[static_cast<int>(CollisionLayer::Enemy)] = 1;
	playerSet[static_cast<int>(CollisionLayer::EnemyBullets)] = 1;
	coordinator.AddComponent(player, SphereCollider{ 25, playerSet, CollisionLayer::Player});

	Entity ent = coordinator.CreateEntity();
	coordinator.AddComponent(ent, Position{ Vector2(250.0f, 300.0f) });
	SDL_Rect rb;
	rb.w = 64;
	rb.h = 64;
	coordinator.AddComponent(ent, Renderer{ "bullet", rb });
	coordinator.AddComponent(ent, Velocity{ Vector2(), 1000.0f });
	CollisionSet enemySet;
	enemySet[static_cast<int>(CollisionLayer::Player)] = 1;
	enemySet[static_cast<int>(CollisionLayer::PlayerBullets)] = 1;
	coordinator.AddComponent(ent, SphereCollider{ 15, enemySet, CollisionLayer::Enemy});

	//printf("\n");
	//CollisionSet a;
	//CollisionSet b;
	//printf(a.to_string().c_str());
	//printf("\n");
	//b[static_cast<int>(CollisionLayer::Default)] = 1;
	//printf(b.to_string().c_str());
	//printf("\n");
	//a[static_cast<int>(CollisionLayer::Default)] = 1;
	//if (a == b)
	//	printf("Collision on Default layer");

	//Init system if needed
	renderSystem->Init();
	bulletSystem->Init(20, player);

	// Game Loop
	while (!bQuit)
	{
		timer->Update();
		while (SDL_PollEvent(&events))
		{
			if (events.type == SDL_QUIT)
				bQuit = true;
		}


		PreUpdate();
		Update();
		renderSystem->Update(timer->DeltaTime());
		inputSystem->Update();
		playerMovementSystem->Update(timer->DeltaTime());
		bulletSystem->Update(timer->DeltaTime());
		movementSystem->Update(timer->DeltaTime());
		sphereCollisionSystem->Update();
		sphereCollisionSystem->DrawDebug();


		//Spawn bullets with an offset of +24 on x from players pos
		LateUpdate();

		Render();
	}
}

void GameManager::RegisterComponents()
{
	coordinator.RegisterComponent<Position>();
	coordinator.RegisterComponent<Renderer>();
	coordinator.RegisterComponent<StaticComponent>();
	coordinator.RegisterComponent<Velocity>();
	coordinator.RegisterComponent<PlayerTag>();
	coordinator.RegisterComponent<InputData>();
	coordinator.RegisterComponent<BulletData>();
	coordinator.RegisterComponent<SphereCollider>();
}

void GameManager::Update()
{


}

void GameManager::LateUpdate()
{
	timer->Reset();
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
