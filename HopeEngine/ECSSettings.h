#pragma once
#include <bitset>

using Entity = std::uint16_t;
const Entity MAX_ENTITIES = 5000;

using ComponentID = std::uint8_t;
const ComponentID MAX_COMPONENTS = 32;
using ComponentList = std::bitset<MAX_COMPONENTS>;


enum class CollisionLayer
{
	Default = 0,
	Player,
	PlayerBullets,
	Enemy,
	EnemyBullets,
	None,


	MaxLayers

};

using CollisionSet = std::bitset<static_cast<int>(CollisionLayer::MaxLayers)>;