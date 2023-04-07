#include "EcoApp.h"

void EcoApp::Update(float dt)
{
	timebank += dt;

	world.Update();
}

void EcoApp::GenerateWorld(EcoResilience::GenerationType genType, int worldWidth, int worldHeight, float maxCellPlantMass)
{
	worldGenType = genType;

	world.Generate(worldWidth, worldHeight, genType, maxCellPlantMass);
	width = worldWidth;
	height = worldHeight;

	timebank = 0;
}