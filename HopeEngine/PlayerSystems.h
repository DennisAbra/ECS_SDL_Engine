#pragma once
#include "System.h"

union SDL_Event;

class RenderSystem : public System
{
public:
	void Init();
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

class PlayerBulletSystem : public System 
{
public: 

	void Init(int amountToSpawn, Entity owner);
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
