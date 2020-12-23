#include "GameManager.h"

int main(int argc, char* args[])
{
	std::unique_ptr<GameManager> gameManager = std::make_unique<GameManager>();

	gameManager->Setup();
	gameManager->UpdateLoop();
	return 0;
}