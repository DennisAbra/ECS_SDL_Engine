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
	float timeBetweenShots = 1;
};

struct BulletData
{
	float timeAlive;
	float maxTimeAlive;
};

struct SphereCollider
{
	float radius;
	CollisionSet collisionSet;
	CollisionLayer collisionID;
	bool wasHit = false;
};

struct Health
{
	int health;
};

struct EnemyTag
{

};

struct EnemyData
{
	float timeSinceShot;
	float timeBetweenShots = 1.0f;
	Entity spawner;
};

struct EnemySpawner
{
	uint8_t deadThisWave;
	uint8_t spawnsPerWave;
	uint8_t amountSpawned;
	uint8_t totalWaves;
	uint8_t currentWave;
	float timeBetweenSpawns;
	float timeSinceSpawned;
};
