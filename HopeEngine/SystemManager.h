#include <memory>
#include "ECSSettings.h"
#include <unordered_map>
#include "System.h"

class SystemManager
{
private:
	std::unordered_map<const char*, ComponentList> componentLists;
	std::unordered_map<const char*, std::shared_ptr<System>> systems{};
public:
	template<typename T>
	std::shared_ptr<T> RegisterSystem()
	{
		const char* typeName = typeid(T).name();

		assert(systems.find(typeName) == systems.end() && "System needs registration before use");

		auto system = std::make_shared<T>();
		systems.insert({ typeName, system });
		return system;
	}

	template<typename T>
	void SetComponentList(ComponentList compList)
	{
		const char* typeName = typeid(T).name();

		assert(systems.find(typeName) != systems.end() && "System needs registration before use");

		componentLists.insert({ typeName, compList });
	}

	void EntityDestroyed(Entity entity)
	{
		for (auto const& pair : systems)
		{
			auto const& system = pair.second;
			system->entities.erase(entity);
		}
	}

	void EntityComponentListChanged(Entity entity, ComponentList compList)
	{
		for (auto const& pair : systems)
		{
			auto const& type = pair.first;
			auto const& system = pair.second;
			auto const& systemComponentList = componentLists[type];

			if ((compList & systemComponentList) == systemComponentList)
			{
				system->entities.insert(entity);
			}
			else
			{
				system->entities.erase(entity);
			}
		}
	}
};