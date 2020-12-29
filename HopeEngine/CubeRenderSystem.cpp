#include "CubeRenderSystem.h"
#include "Coordinator.h"
#include "Position.h"

extern Coordinator coordinator;


void CubeRenderSystem::Update(float dt)
{
	for (Entity const& entity : entities)
	{
		auto& renderer = coordinator.GetComponent<Renderer>(entity);
		auto& position = coordinator.GetComponent<Position>(entity);
		position.location.x += 1 * dt;
		coordinator.DrawCube(position.location, renderer.color, renderer.size);
	}
}
