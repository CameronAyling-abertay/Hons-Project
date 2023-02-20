#include "EcoApp.h"

void EcoApp::Update(float dt)
{
	timebank += dt;

	world.Update();

	float water = 0;
	for(auto cell : world)
	{
		water += cell->GetWater();
	}

	if (water <= width * height / 4.f)
		Rain();
}

void EcoApp::GenerateWorld(EcoResilience::GenerationType genType, int worldWidth, int worldHeight, float maxCellPlantMass)
{
	worldGenType = genType;

	world.Generate(worldWidth, worldHeight, genType, maxCellPlantMass);
	width = worldWidth;
	height = worldHeight;

	timebank = 0;
}