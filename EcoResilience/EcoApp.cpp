#include "EcoApp.h"

void EcoApp::Update(float dt)
{
	timebank += dt;

	world = world.Update();
}

void EcoApp::GenerateWorld(EcoResilience::GenerationType genType, int worldWidth, int worldHeight, float maxCellPlantMass)
{
	worldGenType = genType;
	
	world.Generate(worldWidth, worldHeight, genType, maxCellPlantMass);
	width = worldWidth;
	height = worldHeight;

	timebank = 0;

	drought = false;
}

void EcoApp::Rain()
{
	float water = 0;

	for (auto cell : world)
		water += cell.GetWater();

	if (water < width * height * 0.1f)
		drought = false;

	if (!drought)
		world.Rain();
}

void EcoApp::Flood()
{
	while(!drought)
	{
		float water = 0;

		for (auto cell : world)
		{
			water += cell.GetWater();
			cell.flooded = true;
		}

		if (water < width * height * 0.75f)
			Rain();
		else
			break;
	}
}

void EcoApp::Plague()
{
	int population = 0;
	for(auto cell : world)
	{
		if (cell.hasAnimal)
			population++;
	}

	int zero = rand() % population;

	for(int i = 0; i < width * height; i++)
	{
		if(world[i].hasAnimal)
		{
			zero--;
		}

		if(zero == 0)
		{
			world[i].animal.Infect();
			break;
		}
	}
}

void EcoApp::Fire()
{
	bool started = false;

	while(!started)
	{
		int cellStart = rand() % (width * height);

		if(world[cellStart].hasPlant && world[cellStart].cellType == EcoResilience::CellType::LAND)
		{
			world[cellStart].plants.SetFire();
			started = true;
		}
	}
}
