#pragma once
#include "SDL_image.h"
#include "Vector2.h"

struct Position
{
	Vector2 location;
};

struct Renderer
{
	const char* texName;
	SDL_Rect rect;
};

struct InputData
{
	float x;
	float y;
	bool isShooting;
};

struct Velocity
{
	Vector2 velocity;
	float speed;
};

struct PlayerTag
{

};

struct ShooterData
{
	float timeSinceShot;
};

struct BulletData
{
	Entity owner;
	bool isActive;
	float timeAlive;
};

struct SphereCollider
{
	float radius;
	CollisionSet collisionSet;
	CollisionLayer collisionID;
};
