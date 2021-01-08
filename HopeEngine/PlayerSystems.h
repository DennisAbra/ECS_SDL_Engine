#pragma once
#include "System.h"

union SDL_Event;

class RenderSystem : public System
{
public:
	void Update(float dt);
};

class PlayerInputSystem : public System
{
public:

	void Update();
};

class PlayerMovementSystem : public System
{
public:

	void Update(float dt);
};

class PlayerShootingSystem : public System 
{
public: 

	void Update(float dt);
};

class MovementSystem : public System 
{
public:
	void Update(float dt);
};

class SphereCollisionSystem : public System 
{
public:

	void Update();
	void DrawDebug();
};

class DamageSystem : public System 
{
public:
	void Update();
};

class EnemyMovementSystem : public System
{
public:
	void Update();
};

class EnemySpawningSystem : public System 
{
public:
	void Update(float dt);
};

class EnemyShootingSystem : public System 
{
public:
	void Update(float dt);
};

class KillDeadEnemies : public System 
{
public:
	void Update();
};

class IsPlayerAlive : public System 
{
public:
	void Update();
};

class CleanUpBullets : public System 
{
public:
	void Update(float dt);
};
