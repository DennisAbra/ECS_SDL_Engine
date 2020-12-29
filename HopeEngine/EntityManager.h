#pragma once
#include "ECSSettings.h"
#include <queue>
#include <array>
#include <assert.h>

class EntityManager
{
private:
	
	std::queue<Entity> availableEntities;

	//Array of componentlists where the index corresponds to ent id
	std::array<ComponentList, MAX_ENTITIES> componentLists{};

	//Amount of current active entities - used to keep limits on how many exist
	uint32_t livingEntityCount{};

public:
	EntityManager()
	{
		for (Entity entity = 0; entity < MAX_ENTITIES; entity++)
		{
			availableEntities.push(entity);
		}
	}

	Entity CreateEntity()
	{
		assert(livingEntityCount < MAX_ENTITIES && "Entities overload");

		Entity id = availableEntities.front();
		availableEntities.pop();
		livingEntityCount++;

		return id;
	}

	void DestroyEntity(Entity entity)
	{
		assert(entity < MAX_ENTITIES && "Entity out of bounds");

		componentLists[entity].reset();

		availableEntities.push(entity);
		livingEntityCount--;
	}

	void SetComponentList(Entity entity, ComponentList compList)
	{
		assert(entity < MAX_ENTITIES && "Entity out of bounds");

		componentLists[entity] = compList;
	}

	ComponentList GetComponentList(Entity entity)
	{
		assert(entity < MAX_ENTITIES && "Entity out of bounds");

		return componentLists[entity];
	}
};