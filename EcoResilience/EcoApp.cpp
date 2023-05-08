#include "EcoApp.h"

void EcoApp::Update()
{
	world = world.Update();

	float water = 0;

	for (auto cell : world)
		water += cell.GetWater();

	if (water < width * height * 0.25f)
		drought = false;
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
	if (!drought)
		world.Rain();
}

void EcoApp::Flood()
{
	while(!drought)
	{
		float water = 0;

		for (int cellNum = 0; cellNum < width * height; cellNum++)
		{
			water += world.at(cellNum).GetWater();
			world.at(cellNum).Flood();
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
