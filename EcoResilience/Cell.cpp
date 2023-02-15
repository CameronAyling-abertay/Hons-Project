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