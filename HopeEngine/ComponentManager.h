#pragma once
#include "ECSSettings.h"
#include <unordered_map>
#include <memory>
#include "ComponentArray.h"


class ComponentManager
{
private:

	std::unordered_map<const char*, ComponentType> componentTypes{  };
	std::unordered_map<const char*, std::shared_ptr<IComponentArray>> componentArrays;
	ComponentType nextComponentType{};

	template<typename T>
	std::shared_ptr<ComponentArray<T>> GetComponentArray()
	{
		const char* typeName = typeid(T).name();

		assert(componentTypes.find(typeName) != componentTypes.end() && "Component needs to be registered before use");

		return std::static_pointer_cast<ComponentArray<T>>(componentArrays[typeName]);
	}

public:

	template<typename T>
	void RegisterComponent()
	{
		const char* typeName = typeid(T).name();

		asser(componentTypes.find(typeName) == componentTypes.end() && "Registering an already existing component");

		componentTypes.insert({ typeName, nextComponentType });
		componentArrays.insert({ typeName, std::make_shared<ComponentArray<T>>() });
		nextComponentType++;
	}

	template<typename T>
	ComponentType GetComponentType()
	{
		const char* typeName = typeid(T).name();

		assert(componentTypes.find(typeName) != componentTypes.end() && "Component needs to be registered before use");

		return componentTypes[typeName];
	}

	template<typename T>
	void AddComponent(Entity entity, T component)
	{
		GetComponentArray<T>()->InsertData(entity, component);
	}

	template<typename T>
	void RemoveComponent(Entity entity)
	{
		GetComponentArray<T>()->RemoveData(entity);
	}

	template<typename T>
	T& GetComponent(Entity entity)
	{
		// Notify each component array that an entity has been destroyed
		// If it has a component for that entity, it will remove it
		for (auto const& pair : componentArrays)
		{
			auto const& component = pair.second;

			component->EntityDestroyed(entity);
		}
	}

	void EntityDestroyed(Entity entity)
	{
		for (auto const& pair : componentArrays)
		{
			auto const& component = pair.second;
			component->EntityDestroyed(entity);
		}
	}
};