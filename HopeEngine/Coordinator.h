#pragma once
#include <memory>
#include "ECSSettings.h"
#include "EntityManager.h"
#include "ComponentManager.h"
#include "SystemManager.h"
#include "WindowResource.h"

class Coordinator
{
private:

	//
	std::unique_ptr<ComponentManager> componentManager;
	std::unique_ptr<EntityManager> entityManager;
	std::unique_ptr<SystemManager> systemManager;
	std::unique_ptr<WindowResource> windowResource;
public:
	//

	void Init()
	{
		windowResource = std::make_unique<WindowResource>();
		componentManager = std::make_unique<ComponentManager>();
		systemManager = std::make_unique<SystemManager>();
		entityManager = std::make_unique<EntityManager>();
	}

	void DrawCube(Vector2 pos, SDL_Color col, float size)
	{
		windowResource->DrawSquare(pos, size, col);
	}

	void DrawLine(Vector2 pos1, Vector2 pos2)
	{
		SDL_Color color;
		color.r = 255.0f;
		windowResource->DrawLine(pos1, pos2, color);
	}

	void DrawCircle(int32_t centerX, int32_t centerY, int32_t radius)
	{
		windowResource->DrawCircle(centerX, centerY, radius);
	}

	void Render()
	{
		windowResource->Render();
	}

	SDL_Texture* GetTexture(const char* textureName)
	{
		return windowResource->GetTexture(textureName);
	}

	void LoadNewTexture(std::string fileName, const char* textureName)
	{
		windowResource->LoadNewTexture(fileName, textureName);
	}

	void DrawTexture(SDL_Texture* tex, SDL_Rect* clip, SDL_Rect* rect, float angle, SDL_RendererFlip flip)
	{
		windowResource->DrawTexture(tex, clip, rect, angle, flip);
	}

	void PrintFrameCounter(int i)
	{
		windowResource->PrintBorderFrameCounter(i);
	}

	void RenderClearBackBuffer()
	{
		windowResource->ClearBackBuffer();
	}

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
	T& GetComponent(Entity entity)
	{
		return componentManager->GetComponent<T>(entity);
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
};