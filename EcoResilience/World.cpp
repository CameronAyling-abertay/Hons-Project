#include "World.h"
#include <random>
#include <iostream>

EcoResilience::World::World(int w, int h)
{
	srand(time(0));

	width = w;
	height = h;

	for (int row = 0; row < height; row++)
	{
		for (int column = 0; column < width; column++)
		{
			Cell* newCell = new Cell();

			float water = float((rand() % 1000)) / 1000.f;

			newCell->SetWater(water);

			push_back(newCell);
		}
	}
}

EcoResilience::World::~World()
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