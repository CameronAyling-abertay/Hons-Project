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
	for (int plantNum = 0; plantNum < plants.size(); plantNum++)
	{
		plants[plantNum].Update();

		if (plants[plantNum].wantsFood)
		{
			if (waterLevel - plants[plantNum].stomachMax * 0.05 > 0)
			{
				plants[plantNum].Feed(plants[plantNum].stomachMax * 0.05);
				SetWater(waterLevel - plants[plantNum].stomachMax * 0.05);
			}
		}

		if(plants[plantNum].killMe)
		{
			plants.erase(plants.begin() + plantNum);
		}
	}

	for (int animalNum = 0; animalNum < animals.size(); animalNum++)
		animals[animalNum].Update();
}