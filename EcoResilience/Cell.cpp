#include "Cell.h"

EcoResilience::Cell::Cell(int row, int column, float plantMass) :
	waterLevel(0.5),
	cellRow(row),
	cellColumn(column),
	desiredChildCount(0),
	maxPlantMass(plantMass)
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
	float currentMass = 0;

	for (int plantNum = 0; plantNum < plants.size(); plantNum++)
	{
		plants[plantNum].Update();

		if (plants[plantNum].wantsFood)
		{
			if (waterLevel - plants[plantNum].stomachMax * 0.02 > 0)
			{
				plants[plantNum].Feed(plants[plantNum].stomachMax * 0.7);
				SetWater(waterLevel - plants[plantNum].stomachMax * 0.1);
			}
		}

		if (plants[plantNum].wantsDeath)
		{
			plants.erase(plants.begin() + plantNum);
			plantNum--;
			continue;
		}

		if (plants[plantNum].wantsChild)
		{
			desiredChildCount++;
			plants[plantNum].Reproduce();
		}

		currentMass += plants[plantNum].mass;
	}

	while(currentMass > maxPlantMass)
	{
		float totalVigor = 0;
		for(auto plant : plants)
		{
			totalVigor += 10.f - plant.vigor;
		}

		float killVigor = totalVigor * static_cast<float>(rand() % 1000) / 1000.f;

		for(int plantNum = 0; plantNum < plants.size(); plantNum++)
		{
			killVigor -= 10.f - plants[plantNum].vigor;
			if (killVigor <= 0)
			{
				currentMass -= plants[plantNum].mass;
				plants.erase(plants.begin() + plantNum);
				break;
			}
		}
	}

	for (int animalNum = 0; animalNum < animals.size(); animalNum++)
		animals[animalNum].Update();
}