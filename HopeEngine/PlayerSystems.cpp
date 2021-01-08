#include "PlayerSystems.h"
#include "Coordinator.h"
#include "Position.h"
#include "StaticComponent.h"
#include "PlayerSystems.h"

extern Coordinator coordinator;


void RenderSystem::Update(float dt)
{
	for (Entity const& entity : entities)
	{
		Renderer& renderer = coordinator.GetComponent<Renderer>(entity);
		Position position = coordinator.GetComponent<Position>(entity);

		renderer.rect.x = position.location.x - renderer.rect.w / 2;
		renderer.rect.y = position.location.y - renderer.rect.h / 2;
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
		const Uint8* kb = SDL_GetKeyboardState(NULL);

		input.y = 0;
		input.x = 0;
		input.isShooting = false;

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

void PlayerShootingSystem::Update(float dt)
{
	for (auto& const entity : entities)
	{
		Position& pos = coordinator.GetComponent<Position>(entity);
		ShooterData& data = coordinator.GetComponent<ShooterData>(entity);
		InputData input = coordinator.GetComponent<InputData>(entity);


		if (input.isShooting > 0 && data.timeSinceShot >= data.timeBetweenShots)
		{
			Entity bullet = coordinator.CreateEntity();
			coordinator.AddComponent<Position>(bullet, Position{ pos.location });
			coordinator.AddComponent<BulletData>(bullet, BulletData{ 0.0f, 0.5f });
			coordinator.AddComponent<Velocity>(bullet, Velocity{ Vector2(0,-1), 1000.0f });
			coordinator.AddComponent<PlayerTag>(bullet, PlayerTag{ });
			SDL_Rect r;
			r.w = 16;
			r.h = 16;
			coordinator.AddComponent<Renderer>(bullet, Renderer{ "bullet", r });
			CollisionSet bulletSet;
			bulletSet[static_cast<int>(CollisionLayer::Enemy)] = 1;

			coordinator.AddComponent<SphereCollider>(bullet, SphereCollider{ 5, bulletSet, CollisionLayer::PlayerBullets });

			data.timeSinceShot = 0;
		}

		data.timeSinceShot += dt;
	}
}

void MovementSystem::Update(float dt)
{
	for (auto& const ent : entities)
	{
		Position& pos = coordinator.GetComponent<Position>(ent);
		Velocity vel = coordinator.GetComponent<Velocity>(ent);

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
		SphereCollider& aColl = coordinator.GetComponent<SphereCollider>(a);
		Position aPos = coordinator.GetComponent<Position>(a);
		Velocity aVel = coordinator.GetComponent<Velocity>(a);

		for (auto& b : entities)
		{
			if (a == b)
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
		Health& health = coordinator.GetComponent<Health>(ent);

		if (collider.wasHit)
		{
			health.health -= 1;
			collider.wasHit = false;
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
			coordinator.AddComponent(ent, Position{ Vector2(50.0f, 50.0f) });
			SDL_Rect rb;
			rb.w = 64;
			rb.h = 64;
			coordinator.AddComponent(ent, Renderer{ "bullet", rb });
			coordinator.AddComponent(ent, Velocity{ Vector2(1,1), 250.0f });
			CollisionSet enemySet;

			enemySet[static_cast<int>(CollisionLayer::PlayerBullets)] = 1;
			coordinator.AddComponent(ent, SphereCollider{ 15, enemySet, CollisionLayer::Enemy });
			coordinator.AddComponent(ent, Health{ 1 });
			coordinator.AddComponent(ent, EnemyTag{ });
			coordinator.AddComponent(ent, EnemyData{ 0.0f, 1.0f, e });

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
		EnemyData& ownerData = coordinator.GetComponent<EnemyData>(e);
		Position ownerPos = coordinator.GetComponent<Position>(e);

		if (ownerData.timeSinceShot > ownerData.timeBetweenShots)
		{
			ownerData.timeSinceShot = 0;
			Entity bullet = coordinator.CreateEntity();
			coordinator.AddComponent<Position>(bullet, Position{ ownerPos.location });
			coordinator.AddComponent<BulletData>(bullet, BulletData{ 0.0f, 1.0f });
			coordinator.AddComponent<Velocity>(bullet, Velocity{ Vector2(0.5f,1.0f), 500.0f });
			SDL_Rect r;
			r.w = 16;
			r.h = 16;
			coordinator.AddComponent<Renderer>(bullet, Renderer{ "bullet", r });
			CollisionSet bulletSet;
			bulletSet[static_cast<int>(CollisionLayer::Player)] = 1;

			coordinator.AddComponent<SphereCollider>(bullet, SphereCollider{ 7, bulletSet, CollisionLayer::EnemyBullets });
			coordinator.AddComponent<EnemyTag>(bullet, EnemyTag{  });
		}
		ownerData.timeSinceShot += dt;
	}
}

void KillDeadEnemies::Update()
{
	for (auto& const e : entities)
	{
		EnemyData data = coordinator.GetComponent<EnemyData>(e);
		Health hp = coordinator.GetComponent<Health>(e);

		if (hp.health <= 0)
		{
			EnemySpawner& spawner = coordinator.GetComponent<EnemySpawner>(data.spawner);
			spawner.deadThisWave++;
			coordinator.DestroyEntity(e);
			break;
		}
	}
}

void IsPlayerAlive::Update()
{
	for (auto& const e : entities)
	{
		Health healt = coordinator.GetComponent<Health>(e);

		if (healt.health <= 0)
		{
			printf("You lost");
			coordinator.DestroyEntity(e);
			break;
		}
	}
}


// Stop reusing entities
// Destory and create them at the right time instead

void CleanUpBullets::Update(float dt)
{
	for (auto& const entity : entities)
	{
		BulletData& data = coordinator.GetComponent<BulletData>(entity);
		SphereCollider col = coordinator.GetComponent<SphereCollider>(entity);

		if (data.timeAlive >= data.maxTimeAlive || col.wasHit)
		{
			coordinator.DestroyEntity(entity);
			break;
		}
		data.timeAlive += dt;
	}
}
