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
	std::shared_ptr<PlayerShootingSystem> playerShootingSystem = coordinator.RegisterSystem<PlayerShootingSystem>();
	std::shared_ptr<MovementSystem> movementSystem = coordinator.RegisterSystem<MovementSystem>();
	std::shared_ptr<SphereCollisionSystem> sphereCollisionSystem = coordinator.RegisterSystem<SphereCollisionSystem>();
	std::shared_ptr<DamageSystem> damageSystem = coordinator.RegisterSystem<DamageSystem>();
	std::shared_ptr<EnemyMovementSystem> enemyMovementSystem = coordinator.RegisterSystem<EnemyMovementSystem>();
	std::shared_ptr<EnemySpawningSystem> enemySpawner = coordinator.RegisterSystem<EnemySpawningSystem>();
	std::shared_ptr<EnemyShootingSystem> enemyShooting = coordinator.RegisterSystem<EnemyShootingSystem>();
	std::shared_ptr<KillDeadEnemies> checkDeadEnemies = coordinator.RegisterSystem<KillDeadEnemies>();
	std::shared_ptr<IsPlayerAlive> playerAlive = coordinator.RegisterSystem<IsPlayerAlive>();
	std::shared_ptr<CleanUpBullets> bulletCleaner = coordinator.RegisterSystem<CleanUpBullets>();

	//Register Systems componentLists
	ComponentList compList;
	compList.set(coordinator.GetComponentType<Position>());
	compList.set(coordinator.GetComponentType<Renderer>());
	coordinator.SetSystemComponentList<RenderSystem>(compList);
	compList.set(coordinator.GetComponentType<Velocity>());
	compList.set(coordinator.GetComponentType<InputData>());
	compList.set(coordinator.GetComponentType<PlayerTag>());
	coordinator.SetSystemComponentList<PlayerInputSystem>(compList);
	compList.set(coordinator.GetComponentType<Position>());
	coordinator.SetSystemComponentList<PlayerMovementSystem>(compList);
	coordinator.SetSystemComponentList<PlayerShootingSystem>(compList);

	compList.reset();

	compList.set(coordinator.GetComponentType<Position>());
	compList.set(coordinator.GetComponentType<Velocity>());
	coordinator.SetSystemComponentList<MovementSystem>(compList);

	compList.set(coordinator.GetComponentType<SphereCollider>());
	coordinator.SetSystemComponentList<SphereCollisionSystem>(compList);

	compList.reset();

	compList.set(coordinator.GetComponentType<Health>());
	compList.set(coordinator.GetComponentType<SphereCollider>());
	coordinator.SetSystemComponentList<DamageSystem>(compList);

	compList.reset();

	compList.set(coordinator.GetComponentType<EnemyTag>());
	compList.set(coordinator.GetComponentType<Velocity>());
	compList.set(coordinator.GetComponentType<Position>());
	compList.set(coordinator.GetComponentType<EnemyData>());
	coordinator.SetSystemComponentList<EnemyMovementSystem>(compList);

	compList.reset();

	compList.set(coordinator.GetComponentType<EnemySpawner>());
	coordinator.SetSystemComponentList<EnemySpawningSystem>(compList);

	compList.reset();

	compList.set(coordinator.GetComponentType<Position>());
	compList.set(coordinator.GetComponentType<EnemyData>());
	compList.set(coordinator.GetComponentType<EnemyTag>());
	coordinator.SetSystemComponentList<EnemyShootingSystem>(compList);

	compList.reset();

	compList.set(coordinator.GetComponentType<Health>());
	compList.set(coordinator.GetComponentType<EnemyData>());
	coordinator.SetSystemComponentList<KillDeadEnemies>(compList);

	compList.reset();
	compList.set(coordinator.GetComponentType<Health>());
	compList.set(coordinator.GetComponentType<PlayerTag>());
	coordinator.SetSystemComponentList<IsPlayerAlive>(compList);

	compList.reset();

	compList.set(coordinator.GetComponentType<BulletData>());
	compList.set(coordinator.GetComponentType<SphereCollider>());
	coordinator.SetSystemComponentList<CleanUpBullets>(compList);


	//Create entities

	Entity player = coordinator.CreateEntity();
	coordinator.AddComponent(player, Position{ Vector2(450.0f, 300.0f) });
	SDL_Rect r;
	r.w = 64;
	r.h = 64;
	coordinator.AddComponent(player, Renderer{ "spaceship", r });
	coordinator.AddComponent(player, Velocity{ Vector2(), 500.0f });
	coordinator.AddComponent(player, PlayerTag{});
	coordinator.AddComponent(player, InputData{});
	coordinator.AddComponent(player, ShooterData{0.0f, 0.3f});
	CollisionSet playerSet;
	playerSet[static_cast<int>(CollisionLayer::Enemy)] = 1;
	playerSet[static_cast<int>(CollisionLayer::EnemyBullets)] = 1;
	coordinator.AddComponent(player, SphereCollider{ 25, playerSet, CollisionLayer::Player });
	coordinator.AddComponent(player, Health{ 10 });


	Entity e = coordinator.CreateEntity();
	coordinator.AddComponent<EnemySpawner>(e, EnemySpawner{0, 3, 0, 3, 0, 1.0f, 5.0f});



	float d = 0;
	int frameCount = 0;

	//Test
	const int FPS = 60;
	const int frameDelay = 1000 / FPS;
	uint32_t frameStart;
	int frameTime;

	// Game Loop
	while (!bQuit)
	{
		frameStart = SDL_GetTicks();
		timer->Update();
		while (SDL_PollEvent(&events))
		{
			if (events.type == SDL_QUIT)
				bQuit = true;
		}

		d += timer->DeltaTime();
		frameCount++;
		PreUpdate();
		Update();
		if (d > 1)
		{
			coordinator.PrintFrameCounter(frameCount);
			d = 0;
			frameCount = 0;
		}

		renderSystem->Update(timer->DeltaTime());
		inputSystem->Update();
		playerMovementSystem->Update(timer->DeltaTime());
		playerShootingSystem->Update(timer->DeltaTime());
		movementSystem->Update(timer->DeltaTime());
		sphereCollisionSystem->Update();
		//sphereCollisionSystem->DrawDebug();
		damageSystem->Update();
		enemyMovementSystem->Update();
		enemySpawner->Update(timer->DeltaTime());
		enemyShooting->Update(timer->DeltaTime());
		checkDeadEnemies->Update();
		playerAlive->Update();
		bulletCleaner->Update(timer->DeltaTime());
		LateUpdate();

		Render();

		frameTime = SDL_GetTicks() - frameStart;

		if (frameDelay > frameTime)
		{
			SDL_Delay(frameDelay - frameTime);
		}
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
	coordinator.RegisterComponent<Health>();
	coordinator.RegisterComponent<EnemyTag>();
	coordinator.RegisterComponent<EnemySpawner>();
	coordinator.RegisterComponent<EnemyData>();
	coordinator.RegisterComponent<ShooterData>();
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
