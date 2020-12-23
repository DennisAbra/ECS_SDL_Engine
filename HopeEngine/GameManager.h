#pragma once
#include "SDL_events.h"
#include "WindowResource.h"
#include <memory>
#include "Timer.h"
#include "ECSSettings.h"
#include "EntityManager.h"
#include "ComponentManager.h"
#include "SystemManager.h"


class GameManager
{
public:

	void Setup();

	void UpdateLoop();
	void EarlyUpdate();
	void Update();
	void LateUpdate();

	//
	Entity CreateEntity()
	{
		return entityManager->CreateEntity();
	}

	void DestroyEntity(Entity entity)
	{
		entityManager->DestroyEntity(entity);
		componentManager->EntityDestroyed(entity);
		systemManager->EntityDestroyed(entity);
	}

	template<typename T>
	void RegisterComponent()
	{
		componentManager->RegisterComponent<T>();
	}

	template<typename T>
	void AddComponent(Entity entity, T comp)
	{
		componentManager->AddComponent(entity, comp);
		auto compList = entityManager->GetComponentList(entity);
		compList.set(componentManager->GetComponentType<T>(), true);
		entityManager->SetComponentList(entity, compList);

		systemManager->EntityComponentListChanged(entity, compList);
	}

	template<typename T>
	void RemoveComponent(Entity entity)
	{
		componentManager->RemoveComponent<T>(entity);

		auto compList = entityManager->GetComponentList(entity);
		compList.set(componentManager->GetComponentType<T>(), false);
		entityManager->SetComponentList(entity, compList);

		systemManager->EntityComponentListChanged(entity, compList);
	}

	template<typename T>
	ComponentType GetComponentType()
	{
		return componentManager->GetComponentType<T>();
	}

	template<typename T>
	std::shared_ptr<T> RegisterSystem()
	{
		return systemManager->RegisterSystem<T>();
	}

	template<typename T>
	void SetSystemComponentList(ComponentList compList)
	{
		systemManager->SetComponentList<T>(compList);
	}

private:
	SDL_Event events;
	bool bQuit = false;

	int frames = 0;
	float time = 0;

	void PreUpdate();
	void Render();

	std::unique_ptr<WindowResource> windowResource;
	std::unique_ptr<Timer> timer;

	//
	std::unique_ptr<ComponentManager> componentManager;
	std::unique_ptr<EntityManager> entityManager;
	std::unique_ptr<SystemManager> systemManager;
};