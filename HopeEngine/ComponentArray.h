#pragma once
#include "ECSSettings.h"

class IComponentArray
{
public:
	virtual ~IComponentArray() = default;
	virtual void EntityDestroyed(Entity entity) = 0;
};

template<typename T>
class ComponentArray : public IComponentArray
{
private:
	//Packed array of components, specified to max entities so each entity has it's specific spot
	std::array<T, MAX_ENTITIES> componentArray;

	//Ent id to array index
	std::unordered_map<Entity, size_t> entityToIndexMap;
	//Array index to ent id
	std::unordered_map<size_t, Entity> indexToEntityMap;
	//total size of valid entries in array
	size_t size;
public:
	void InsertData(Entity entity, T component)
	{
		assert(entityToIndexMap.find(entity) == entityToIndexMap.end() && "Same component added twice to same entity");

		size_t newIndex = size;
		entityToIndexMap[entity] = newIndex;
		indexToEntityMap[newIndex] = entity;
		componentArray[newIndex] = component;
		++size;
	}

	void RemoveData(Entity entity)
	{
		assert(entityToIndexMap.find(entity) != entityToIndexMap.end() && "Removing non-existent component");

		size_t indexOfRemovedEntity = entityToIndexMap[entity];
		size_t indexOfLastElement = size - 1;
		componentArray[indexOfRemovedEntity] = componentArray[indexOfLastElement];

		Entity entityOfLastElement = indexToEntityMap[indexOfLastElement];
		entityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
		indexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

		entityToIndexMap.erase(entity);
		indexToEntityMap.erase(indexOfLastElement);

		--size;
	}

	T& GetData(Entity entity)
	{
		assert(entityToIndexMap.find(entity) != entityToIndexMap.end() && "Trying to find non-existent component");

		return componentArray[entityToIndexMap[entity]];
	}

	void EntityDestroyed(Entity entity) override
	{
		if (entityToIndexMap.find(entity) != entityToIndexMap.end())
			RemoveData(entity);
	}
};