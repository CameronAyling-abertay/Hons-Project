#include "World.h"
#include <random>

EcoResilience::World::World() :
	width(0),
	height(0),
	sunTime(0)
{}

EcoResilience::World::~World()
{
	Clear();
}

void EcoResilience::World::Generate(int w, int h, GenerationType type)
{
	Clear();

	width = w;
	height = h;

	//Generate the world
	//Random generation will randomly allocate cells water levels
	//Perlin generation will generate it using perlin noise for better natural-ness
	switch (type)
	{
	case GenerationType::RANDOM:
		sunTime = float(rand() % 1000) / 24.f;

		for (int row = 0; row < height; row++)
		{
			for (int column = 0; column < width; column++)
			{
				Cell* newCell = new Cell();

				float water = float((rand() % 1000)) / 1000.f;

				newCell->SetWater(water);
				
				int plantPop = rand() % 10;

				for (int plantNum = 0; plantNum < plantPop; plantNum++)
				{
					newCell->AddPlant();
				}

				push_back(newCell);
			}
		}
		break;
	}
}

void EcoResilience::World::Clear()
{
	for (int row = 0; row < height; row++)
	{
		for (int column = 0; column < width; column++)
		{
			delete back();
			pop_back();
		}
	}
}

void EcoResilience::World::Update()
{
	for (int cellNum = 0; cellNum < width * height; cellNum++)
	{
		this->data()[cellNum]->Update();
	}
}

int EcoResilience::World::GetPopulation(PopulationType type)
{
	int population = 0;

	for (int cellNum = 0; cellNum < size(); cellNum++)
	{
		population += data()[cellNum]->GetPopulation(type);
	}

	return population;
}