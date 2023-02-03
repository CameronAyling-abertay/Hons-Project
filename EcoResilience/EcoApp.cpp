#include "EcoApp.h"

void EcoApp::Update(float dt)
{
	timebank += dt;

	if (timebank > 0.5f)
	{
		timebank -= 0.5f;

	}
}

void EcoApp::GenerateWorld(EcoResilience::GenerationType genType, int worldWidth, int worldHeight)
{
	world.Generate(worldWidth, worldHeight, genType);
	width = worldWidth;
	height = worldHeight;

	timebank = 0;

	for (auto cell : world)
	{
		for (auto animal : cell->animals)
		{
			animals.push_back(&animal);
		}
	}
}