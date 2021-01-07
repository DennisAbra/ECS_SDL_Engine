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
	std::shared_ptr<DamageSystem> healthSystem = coordinator.RegisterSystem<DamageSystem>();
	std::shared_ptr<EnemyMovementSystem> enemyMovementSystem = coordinator.RegisterSystem<EnemyMovementSystem>();
	std::shared_ptr<EnemySpawningSystem> enemySpawner = coordinator.RegisterSystem<EnemySpawningSystem>();
	std::shared_ptr<EnemyShootingSystem> enemyShooting = coordinator.RegisterSystem<EnemyShootingSystem>();
	std::shared_ptr<CheckForDeadEnemies> checkDeadEnemies = coordinator.RegisterSystem<CheckForDeadEnemies>();
	std::shared_ptr<IsPlayerAlive> playerAlive = coordinator.RegisterSystem<IsPlayerAlive>();

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

	compList.reset();

	compList.set(coordinator.GetComponentType<Velocity>());
	compList.set(coordinator.GetComponentType<BulletData>());
	compList.set(coordinator.GetComponentType<Position>());
	compList.set(coordinator.GetComponentType<PlayerTag>());
	coordinator.SetSystemComponentList<PlayerBulletSystem>(compList);

	compList.reset();

	compList.set(coordinator.GetComponentType<Position>());
	compList.set(coordinator.GetComponentType<Velocity>());
	coordinator.SetSystemComponentList<MovementSystem>(compList);

	compList.set(coordinator.GetComponentType<SphereCollider>());
	coordinator.SetSystemComponentList<SphereCollisionSystem>(compList);

	compList.reset();

	compList.set(coordinator.GetComponentType<Health>());
	compList.set(coordinator.GetComponentType<SphereCollider>());
	compList.set(coordinator.GetComponentType<Active>());
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
	compList.set(coordinator.GetComponentType<Velocity>());
	compList.set(coordinator.GetComponentType<BulletData>());
	compList.set(coordinator.GetComponentType<Active>());
	compList.set(coordinator.GetComponentType<EnemyTag>());
	coordinator.SetSystemComponentList<EnemyShootingSystem>(compList);

	compList.reset();

	compList.set(coordinator.GetComponentType<Health>());
	compList.set(coordinator.GetComponentType<EnemyData>());
	coordinator.SetSystemComponentList<CheckForDeadEnemies>(compList);

	compList.reset();
	compList.set(coordinator.GetComponentType<Health>());
	compList.set(coordinator.GetComponentType<PlayerTag>());
	compList.set(coordinator.GetComponentType<Active>());
	coordinator.SetSystemComponentList<IsPlayerAlive>(compList);


	//Create entities

	Entity player = coordinator.CreateEntity();
	coordinator.AddComponent(player, Position{ Vector2(450.0f, 300.0f) });
	SDL_Rect r;
	r.w = 64;
	r.h = 64;
	coordinator.AddComponent(player, Renderer{ "spaceship", r });
	coordinator.AddComponent(player, Velocity{ Vector2(), 1000.0f });
	coordinator.AddComponent(player, PlayerTag{});
	coordinator.AddComponent(player, InputData{});
	CollisionSet playerSet;
	playerSet[static_cast<int>(CollisionLayer::Enemy)] = 1;
	playerSet[static_cast<int>(CollisionLayer::EnemyBullets)] = 1;
	coordinator.AddComponent(player, SphereCollider{ 25, playerSet, CollisionLayer::Player });
	coordinator.AddComponent(player, Active{ true });
	coordinator.AddComponent(player, Health{ 10 });


	Entity e = coordinator.CreateEntity();
	coordinator.AddComponent<EnemySpawner>(e, EnemySpawner{0, 3, 0, 3, 0, 1.0f, 5.0f});


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
		//sphereCollisionSystem->DrawDebug();
		healthSystem->Update();
		enemyMovementSystem->Update();
		enemySpawner->Update(timer->DeltaTime());
		enemyShooting->Update(timer->DeltaTime());
		checkDeadEnemies->Update();
		playerAlive->Update();
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
	coordinator.RegisterComponent<Active>();
	coordinator.RegisterComponent<Health>();
	coordinator.RegisterComponent<EnemyTag>();
	coordinator.RegisterComponent<EnemySpawner>();
	coordinator.RegisterComponent<EnemyData>();
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
