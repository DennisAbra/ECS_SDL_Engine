#pragma once
#include "ECSSettings.h"
#include <unordered_map>
#include <memory>
#include "ComponentArray.h"


class ComponentManager
{
private:

	std::array <std::shared_ptr<ComponentArrayHead>, MAX_COMPONENTS> componentArray;
	ComponentType nrOfComponentTypes = 0;

	template<typename T>
	std::shared_ptr<ComponentArray<T>> GetComponentArray()
	{
		return std::static_pointer_cast<ComponentArray<T>>(componentArray[GetComponentType<T>()]);
	}

public:

	template<typename T>
	void RegisterComponent()
	{
		componentArray[GetComponentType<T>()] = std::make_shared<ComponentArray<T>>();
	}

	template<typename T>
	void AddComponent(Entity entity, T& component)
	{
		GetComponentArray<T>()->AddComponent(entity, component);
	}

	template<typename T>
	T& GetComponent(Entity entity)
	{
		return GetComponentArray<T>()->GetComponent(entity);
	}

	template<typename T>
	void RemoveComponent(Entity entity)
	{
		GetComponentArray<T>()->RemoveComponent(entity);
	}

	
	void EntityDestroyed(Entity entity)
	{
		
	}

	template<typename T>
	ComponentType GetComponentType()
	{
		const static ComponentType compType = nrOfComponentTypes++;
		return compType;
	}

	template<typename T>
	size_t GetNumberOfActiveComponents()
	{
		GetComponentArray<T>()->GetNumberOfActiveComponents();
	}
};