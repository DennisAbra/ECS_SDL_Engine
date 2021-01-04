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

		renderer.rect.x = position.location.x - renderer.rect.w/2;
		renderer.rect.y = position.location.y - renderer.rect.h/2;
		coordinator.DrawTexture(coordinator.GetTexture(renderer.texName), &renderer.rect, &renderer.rect, 0, SDL_FLIP_NONE);
	}
}


void PlayerInputSystem::Update()
{
	for (Entity const& entity : entities)
	{
		InputData& input = coordinator.GetComponent<InputData>(entity);

		const Uint8* kb = SDL_GetKeyboardState(NULL);

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
		coordinator.AddComponent<BulletData>(bullet, BulletData{ owner, false, 0 });
		coordinator.AddComponent<Velocity>(bullet, Velocity{ Vector2(0,-1), 1500 });
		SDL_Rect r;
		r.w = 16;
		r.h = 16;
		coordinator.AddComponent<Renderer>(bullet, Renderer{ "bullet", r });
		CollisionSet bulletSet;
		bulletSet[static_cast<int>(CollisionLayer::Enemy)] = 1;

		coordinator.AddComponent<SphereCollider>(bullet, SphereCollider{5, bulletSet, CollisionLayer::PlayerBullets});
	}
}

void PlayerBulletSystem::Update(float dt)
{
	static float t = 0;
	for (auto& bullet : entities)
	{
		BulletData& data = coordinator.GetComponent<BulletData>(bullet);
		Position& pos = coordinator.GetComponent<Position>(bullet);
		InputData input = coordinator.GetComponent<InputData>(data.owner);
		Position playerPos = coordinator.GetComponent<Position>(data.owner);
		//Get ShooterData
		//ShooterData should implement T

		if (input.isShooting > 0 && !data.isActive && t <= 0)
		{
			data.timeAlive = 0;
			data.isActive = true;
			pos.location = playerPos.location;
			t = 1;
		}

		if (data.isActive)
		{
			data.timeAlive += dt;
			if (data.timeAlive > 1)
			{
				data.isActive = false;
				pos.location = Vector2(-100, -100);
			}
		}
		t -= dt;
	}
}

void MovementSystem::Update(float dt)
{
	for (auto& ent : entities)
	{
		Position& pos = coordinator.GetComponent<Position>(ent);
		Velocity vel = coordinator.GetComponent<Velocity>(ent);

		pos.location += vel.velocity * vel.speed * dt;
	}
}

void SphereCollisionSystem::Update()
{
	for (auto& a : entities)
	{
		Position aPos = coordinator.GetComponent<Position>(a);
		Velocity aVel = coordinator.GetComponent<Velocity>(a);
		SphereCollider aColl = coordinator.GetComponent<SphereCollider>(a);
		for (auto& b : entities)
		{
			if (a == b)
				continue;
			Position bPos = coordinator.GetComponent<Position>(b);
			Velocity bVel = coordinator.GetComponent<Velocity>(b);
			SphereCollider bColl = coordinator.GetComponent<SphereCollider>(b);

			if (aColl.collisionSet[static_cast<int>(bColl.collisionID)] == 0)
				continue;
			if (bColl.collisionSet[static_cast<int>(aColl.collisionID)] == 0)
				continue;

			Vector2 s = aPos.location - bPos.location;
			Vector2 v = aVel.velocity - bVel.velocity;
			float radiusSum = aColl.radius + bColl.radius;
			float dist = s.Dot(s) - radiusSum * radiusSum;

			if (dist < 0.0f)
			{
				//printf("Will collide\n");

				float distance = s.Magnitude();
				if (distance < radiusSum)
				{
					printf("Collision Happened\n");
					coordinator.DrawLine(aPos.location, bPos.location);
				}
			}

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
				printf("Collision Happened\n");
				coordinator.DrawLine(aPos.location, bPos.location);
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
