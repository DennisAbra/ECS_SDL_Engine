#pragma once
#include "ECSSettings.h"

class ComponentArrayHead{};

template<typename T>
class ComponentArray : public ComponentArrayHead
{
private:
	std::array<T, MAX_ENTITIES> componentArray;
	std::uint16_t currentActiveComponets = 0;

public:
	
	void AddComponent(Entity entity, T& component)
	{
		componentArray[entity] = component;
		currentActiveComponets++;
	}

	T& GetComponent(Entity entity)
	{
		assert(entity <= MAX_ENTITIES && "Entity out of bounds");
		return componentArray[entity];
	}

	size_t GetNumberOfActiveComponents()
	{
		currentActiveComponets;
	}

	void RemoveComponent(Entity entity)
	{
		assert(entity <= MAX_ENTITIES && "Entity out of bounds");
		componentArray[entity] = componentArray[currentActiveComponets];
		currentActiveComponets--;
	}

};
