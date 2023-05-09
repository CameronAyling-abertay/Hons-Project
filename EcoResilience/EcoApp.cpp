#include "EcoApp.h"

EcoApp::EcoApp() :
	drought(false),
	worldGenType(EcoResilience::GenerationType::RANDOM),
	width(DEFAULT_SIDE),
	height(DEFAULT_SIDE)
{}

void EcoApp::Update()
{
	//Set the world reference to the back frame
	world = world.Update();

	//Find the amount of water in the world to determine whether the drought is over
	float water = 0;

	for (auto cell : world)
		water += cell.GetWater();

	if (water < width * height * 0.25f)
		drought = false;
}

void EcoApp::GenerateWorld(EcoResilience::EcoSize size, EcoResilience::EcoParameters parameters)
{
	//Generate the world
	worldGenType = parameters.genType;
	
	world.Generate(size, parameters);
	width = size.width;
	height = size.height;

	//Set the drought flag to false
	drought = false;
}

void EcoApp::Rain()
{
	//If the world is not in drought, let it rain
	if (!drought)
		world.Rain();
}

void EcoApp::Flood()
{
	//If the world is not in drought, let it rain until the world is flooded
	while(!drought)
	{
		//Find the amount of water in the world to determine whether it is flooded
		float water = 0;

		for (int cellNum = 0; cellNum < width * height; cellNum++)
		{
			water += world.at(cellNum).GetWater();
			world.at(cellNum).Flood();
		}

		//If the world is not flooded, make it rain
		if (water < width * height * 0.85f)
			Rain();
		else//End the cycle otherwise as it is flooded
			break;
	}
}

void EcoApp::Plague()
{
	//Find how many animals are in the world
	int population = 0;
	for(auto cell : world)
	{
		if (cell.hasAnimal)
			population++;
	}

	//Determine an index to be patient zero
	int zero = rand() % population;

	//Find patient zero
	for(int i = 0; i < width * height; i++)
	{
		if(world[i].hasAnimal)
			zero--;

		if(zero == 0)
		{
			//Infect patient zero
			world[i].InfectAnimal();
			break;
		}
	}
}

void EcoApp::Fire()
{
	//Find a cell with plants to light on fire
	bool started = false;

	while(!started)
	{
		int cellStart = rand() % (width * height);

		if(world[cellStart].hasPlant && world[cellStart].cellType == EcoResilience::CellType::LAND)
		{
			//Light the plants in the cell on fire
			world[cellStart].SetFire();
			started = true;
		}
	}
}
