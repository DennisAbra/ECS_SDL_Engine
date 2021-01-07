#include "PlayerSystems.h"
#include "Coordinator.h"
#include "Position.h"
#include "StaticComponent.h"
#include "PlayerSystems.h"

extern Coordinator coordinator;


void RenderSystem::Init()
{
	//for (Entity const& entity : entities)
	//{
	//	Renderer& renderer = coordinator.GetComponent<Renderer>(entity);

	//}
}

void RenderSystem::Update(float dt)
{
	for (Entity const& entity : entities)
	{
		Renderer& renderer = coordinator.GetComponent<Renderer>(entity);
		Position position = coordinator.GetComponent<Position>(entity);
		Active active = coordinator.GetComponent<Active>(entity);

		renderer.rect.x = position.location.x - renderer.rect.w / 2;
		renderer.rect.y = position.location.y - renderer.rect.h / 2;
		if (active.isActive)
			coordinator.DrawTexture(coordinator.GetTexture(renderer.texName), &renderer.rect, &renderer.rect, 0, SDL_FLIP_NONE);
	}
}


void PlayerInputSystem::Update()
{
	//Limit this with a playertag
	//That way I can reuse input for Enemies
	//By creating a movementsystem for entities with another tag using only velocity and input to read from
	for (Entity const& entity : entities)
	{
		InputData& input = coordinator.GetComponent<InputData>(entity);
		Active& active = coordinator.GetComponent<Active>(entity);
		const Uint8* kb = SDL_GetKeyboardState(NULL);
		if (!active.isActive)
		{
			input.y = 0;
			input.x = 0;
			input.isShooting = false;
			continue;
		}

		input.y = -kb[SDL_SCANCODE_W] + kb[SDL_SCANCODE_S];
		input.x = -kb[SDL_SCANCODE_A] + kb[SDL_SCANCODE_D];
		kb[SDL_SCANCODE_SPACE] > 0 ? input.isShooting = true : input.isShooting = false;
	}
}

void PlayerMovementSystem::Update(float dt)
{
	for (Entity const& entity : entities)
	{
		Velocity& velocity = coordinator.GetComponent<Velocity>(entity);
		InputData& input = coordinator.GetComponent<InputData>(entity);

		velocity.velocity.x = input.x;
		velocity.velocity.y = input.y;
	}
}

void PlayerBulletSystem::Init(int amountToSpawn, Entity owner)
{
	for (int i = 0; i < amountToSpawn; i++)
	{
		Entity bullet = coordinator.CreateEntity();
		coordinator.AddComponent<Position>(bullet, Position{ Vector2(-100,-100) });
		coordinator.AddComponent<BulletData>(bullet, BulletData{ owner, false });
		coordinator.AddComponent<Velocity>(bullet, Velocity{ Vector2(0,-1), 1500 });
		coordinator.AddComponent<PlayerTag>(bullet, PlayerTag{ });
		SDL_Rect r;
		r.w = 16;
		r.h = 16;
		coordinator.AddComponent<Renderer>(bullet, Renderer{ "bullet", r });
		CollisionSet bulletSet;
		bulletSet[static_cast<int>(CollisionLayer::Enemy)] = 1;

		coordinator.AddComponent<SphereCollider>(bullet, SphereCollider{ 5, bulletSet, CollisionLayer::None });
		coordinator.AddComponent<Active>(bullet, Active{ false });
	}
}

void PlayerBulletSystem::Update(float dt)
{
	static float t = 0;
	for (auto& const bullet : entities)
	{
		BulletData& data = coordinator.GetComponent<BulletData>(bullet);
		Position& pos = coordinator.GetComponent<Position>(bullet);
		Active& active = coordinator.GetComponent<Active>(bullet);
		SphereCollider& col = coordinator.GetComponent<SphereCollider>(bullet);
		InputData input = coordinator.GetComponent<InputData>(data.owner);
		Position playerPos = coordinator.GetComponent<Position>(data.owner);
		//Get ShooterData
		//ShooterData should implement T as a shootingCooldown

		if (input.isShooting > 0 && !active.isActive && t <= 0)
		{
			data.timeAlive = 0;
			active.isActive = true;
			col.collisionID = CollisionLayer::PlayerBullets;
			pos.location = playerPos.location;
			t = 1;
		}

		if (active.isActive)
		{
			data.timeAlive += dt;
			if (data.timeAlive > 1)
			{
				active.isActive = false;
			}
		}

		if (!active.isActive)
		{
			pos.location = Vector2(50, 50);
		}
		t -= dt;
	}
}

void MovementSystem::Update(float dt)
{
	for (auto& const ent : entities)
	{
		Position& pos = coordinator.GetComponent<Position>(ent);
		Velocity vel = coordinator.GetComponent<Velocity>(ent);
		Active active = coordinator.GetComponent<Active>(ent);


		if (active.isActive)
			pos.location += vel.velocity * vel.speed * dt;
	}
}

void SphereCollisionSystem::Update()
{
	//Does not scale well rethink this
	//Should check if we already collided with this entity 
	// To avoid several collision per occasion

	for (auto& const a : entities)
	{
		Active& aActive = coordinator.GetComponent<Active>(a);
		if (!aActive.isActive)
			continue;

		SphereCollider& aColl = coordinator.GetComponent<SphereCollider>(a);
		Position aPos = coordinator.GetComponent<Position>(a);
		Velocity aVel = coordinator.GetComponent<Velocity>(a);

		for (auto& b : entities)
		{
			if (a == b)
				continue;
			Active& bActive = coordinator.GetComponent<Active>(b);
			if (!bActive.isActive)
				continue;
			SphereCollider& bColl = coordinator.GetComponent<SphereCollider>(b);

			if (aColl.collisionSet[static_cast<int>(bColl.collisionID)] == 0)
				continue;
			if (bColl.collisionSet[static_cast<int>(aColl.collisionID)] == 0)
				continue;

			//Above - CollisionQuery
			//Under - WillBeCollision
			Position bPos = coordinator.GetComponent<Position>(b);
			Velocity bVel = coordinator.GetComponent<Velocity>(b);

			Vector2 s = aPos.location - bPos.location;
			Vector2 v = aVel.velocity - bVel.velocity;
			float radiusSum = aColl.radius + bColl.radius;
			float dist = s.Dot(s) - radiusSum * radiusSum;

			float vDotV = v.Dot(v);
			float vDotS = v.Dot(s);
			if (vDotS >= 0.0f)
			{
				continue;
				//printf("Spheres not on collision path\n");
			}

			if (vDotS * vDotS - vDotV * dist < 0.0f)
			{
				continue;
				//printf("Spheres not on collision path\n");
			}

			//printf("Will collide\n");
			float distance = s.Magnitude();
			if (distance < radiusSum)
			{
				//Collision happened
				aColl.wasHit = true;
				bColl.wasHit = true;
				coordinator.DrawLine(aPos.location, bPos.location);
				if (aColl.collisionID == CollisionLayer::PlayerBullets || aColl.collisionID == CollisionLayer::EnemyBullets)
				{
					aActive.isActive = false;
				}
				if (bColl.collisionID == CollisionLayer::PlayerBullets || bColl.collisionID == CollisionLayer::EnemyBullets)
				{
					bActive.isActive = false;
				}
				continue;
			}
		}
	}
}

void SphereCollisionSystem::DrawDebug()
{
	for (auto& ent : entities)
	{
		SphereCollider aColl = coordinator.GetComponent<SphereCollider>(ent);
		Position aPos = coordinator.GetComponent<Position>(ent);
		coordinator.DrawCircle(aPos.location.x, aPos.location.y, aColl.radius);

	}
}

void DamageSystem::Update()
{
	for (auto& const ent : entities)
	{
		SphereCollider& collider = coordinator.GetComponent<SphereCollider>(ent);
		Active& active = coordinator.GetComponent<Active>(ent);
		Health& health = coordinator.GetComponent<Health>(ent);

		if (collider.wasHit)
		{
			health.health -= 1;
			collider.wasHit = false;
			//if (health.health <= 0)
			//{
			//	active.isActive = false;
			//}
		}
	}
}

void EnemyMovementSystem::Update()
{
	for (auto& const e : entities)
	{
		Velocity& velocity = coordinator.GetComponent<Velocity>(e);
		Position pos = coordinator.GetComponent<Position>(e);

		if (pos.location.x >= 1280)
		{
			velocity.velocity.x = -1;
		}

		if (pos.location.x <= 0)
		{
			velocity.velocity.x = 1;
		}

		if (pos.location.y >= 520)
		{
			velocity.velocity.y = -1;
		}

		if (pos.location.y <= 0)
		{
			velocity.velocity.y = 1;
		}
	}
}

void EnemySpawningSystem::Update(float dt)
{
	for (auto& const e : entities)
	{
		EnemySpawner& enemySpawner = coordinator.GetComponent<EnemySpawner>(e);

		if (enemySpawner.currentWave == enemySpawner.totalWaves && enemySpawner.amountSpawned == enemySpawner.amountSpawned)
		{
			//Gone through all waves and finished the spawns on the last wave.
			printf("VICTORY!");
			coordinator.DestroyEntity(e);
			break;
		}

		if (enemySpawner.timeSinceSpawned >= enemySpawner.timeBetweenSpawns && enemySpawner.amountSpawned < enemySpawner.spawnsPerWave)
		{
			Entity ent = coordinator.CreateEntity();
			coordinator.AddComponent(ent, Position{ Vector2(-20.0f, 50.0f) });
			SDL_Rect rb;
			rb.w = 64;
			rb.h = 64;
			coordinator.AddComponent(ent, Renderer{ "bullet", rb });
			coordinator.AddComponent(ent, Velocity{ Vector2(0,1), 250.0f });
			CollisionSet enemySet;

			enemySet[static_cast<int>(CollisionLayer::PlayerBullets)] = 1;
			coordinator.AddComponent(ent, SphereCollider{ 15, enemySet, CollisionLayer::Enemy });
			coordinator.AddComponent(ent, Active{ true });
			coordinator.AddComponent(ent, Health{ 5 });
			coordinator.AddComponent(ent, EnemyTag{ });
			coordinator.AddComponent(ent, EnemyData{ 1.0f, 1.0f, e });

			for (int i = 0; i < 3; i++)
			{
				//Spawn bullet this enemy will use
				Entity bullet = coordinator.CreateEntity();
				coordinator.AddComponent<Position>(bullet, Position{ Vector2(-100,-100) });
				coordinator.AddComponent<BulletData>(bullet, BulletData{ ent, false });
				coordinator.AddComponent<Velocity>(bullet, Velocity{ Vector2(1,1), 1000.0f });
				SDL_Rect r;
				r.w = 16;
				r.h = 16;
				coordinator.AddComponent<Renderer>(bullet, Renderer{ "bullet", r });
				CollisionSet bulletSet;
				bulletSet[static_cast<int>(CollisionLayer::Player)] = 1;

				coordinator.AddComponent<SphereCollider>(bullet, SphereCollider{ 5, bulletSet, CollisionLayer::EnemyBullets });
				coordinator.AddComponent<Active>(bullet, Active{ false });
				coordinator.AddComponent<EnemyTag>(bullet, EnemyTag{  });

			}
			enemySpawner.timeSinceSpawned = 0;
			enemySpawner.amountSpawned++;
		}

		if (enemySpawner.deadThisWave == enemySpawner.spawnsPerWave && enemySpawner.amountSpawned == enemySpawner.spawnsPerWave)
		{
			enemySpawner.currentWave++;
			enemySpawner.deadThisWave = 0;
			enemySpawner.amountSpawned = 0;
		}

		enemySpawner.timeSinceSpawned += dt;
	}
}

void EnemyShootingSystem::Update(float dt)
{
	for (auto& const e : entities)
	{
		BulletData data = coordinator.GetComponent<BulletData>(e);
		EnemyData& ownerData = coordinator.GetComponent<EnemyData>(data.owner);
		Position ownerPos = coordinator.GetComponent<Position>(data.owner);
		Velocity& velocity = coordinator.GetComponent<Velocity>(e);
		Position& pos = coordinator.GetComponent<Position>(e);
		Active& active = coordinator.GetComponent<Active>(e);

		Active ownerActive = coordinator.GetComponent<Active>(data.owner);
		if (!ownerActive.isActive)
			active.isActive = false;

		if (ownerData.timeSinceShot > ownerData.timeBetweenShots)
		{
			active.isActive = true;
			velocity.velocity = Vector2(0.5f, 1);
			pos.location = ownerPos.location;
			ownerData.timeSinceShot = 0;
		}

		if (active.isActive)
		{
			data.timeAlive++;
			if (data.timeAlive > 1)
			{
				pos.location = Vector2(1270, 50);
				active.isActive = false;
				data.timeAlive = 0;
			}
		}
		ownerData.timeSinceShot += dt;
	}
}

void CheckForDeadEnemies::Update()
{
	for (auto& const e : entities)
	{
		EnemyData data = coordinator.GetComponent<EnemyData>(e);
		Health hp = coordinator.GetComponent<Health>(e);
		Active& active = coordinator.GetComponent<Active>(e);

		if (hp.health <= 0 && active.isActive)
		{
			EnemySpawner& spawner = coordinator.GetComponent<EnemySpawner>(data.spawner);
			spawner.deadThisWave++;
			active.isActive = false;
		}
	}
}

void IsPlayerAlive::Update()
{
	for (auto& const e : entities)
	{
		Active& active = coordinator.GetComponent<Active>(e);
		Health healt = coordinator.GetComponent<Health>(e);

		if (healt.health <= 0)
		{
			active.isActive = false;
			printf("You lost");
			coordinator.RemoveComponent<Health>(e);
			break;
		}
	}
}
