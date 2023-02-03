#include "Cell.h"

EcoResilience::Cell::Cell(int row, int column) :
	waterLevel(0.5),
	cellRow(row),
	cellColumn(column)
{
}

int EcoResilience::Cell::GetPopulation(PopulationType type)
{
	switch (type)
	{
	case PopulationType::PLANT:
		return plants.size();
	case PopulationType::PREY:
		return animals.size();
	}
}

void EcoResilience::Cell::Update()
{
	for (auto plant : plants)
		plant.Update();

	for (auto animal : animals)
		animal.Update();
}

void EcoResilience::Cell::MoveAnimal(int direction, Animal* animal)
{
	switch (direction)
	{
	case 1: //North
		break;
	case 2: //East
		break;
	case 3: //South
		break;
	case 4: // West
		break;
	}
}