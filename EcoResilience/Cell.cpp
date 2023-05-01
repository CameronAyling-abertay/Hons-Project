#include "Cell.h"

EcoResilience::Cell::Cell(int row, int column, float plantMass) :
	waterLevel(0.5),
	cellRow(row),
	cellColumn(column),
	plants(NULL),
	plantWantsChild(false),
	animalWantsMove(false),
	maxPlantMass(plantMass)
{
}

int EcoResilience::Cell::GetPopulation(PopulationType type)
{
	if (animal)
	{
		switch (type)
		{
		case PopulationType::PREY:
			if (animal->type == PopulationType::PREY)
				return 1;
			return 0;

		case PopulationType::PREDATOR:
			if (animal->type == PopulationType::PREDATOR)
				return 1;
			return 0;
		}
	}
	return 0;
}

void EcoResilience::Cell::AddPlant(Plant plant)
{
	plants = new Plant();
	*plants = plant;
}

void EcoResilience::Cell::AddAnimal(Animal fauna)
{
	animal = new Animal(PopulationType::PREY);
	*animal = fauna;
}

void EcoResilience::Cell::SetWater(float newWaterLevel)
{
	waterLevel = std::min(1.f, std::max(0.f, newWaterLevel));
	waterLevel * 255.f > 150.f ? cellType = CellType::WATER : cellType = CellType::LAND;
}


void EcoResilience::Cell::Update()
{
	if (cellType == CellType::WATER)
	{
		if (animal)
		{
			delete animal;
			animal = 0;
		}
	}
	else if (cellType == CellType::LAND)
	{
		if (plants)
		{
			plants->Update();

			if (plants->wantsFood)
			{
				float mod = -0.5 + rand() % 1000 / 1000.f;

				if (waterLevel - plants->stomachMax * (0.02 + 0.02 * mod) > 0)
				{
					plants->Feed(plants->stomachMax * (0.6 + 0.2 * mod));
					SetWater(waterLevel - plants->stomachMax * (0.02 + 0.02 * mod));
				}
			}

			plantWantsChild = plants->wantsChild;

			plants->mass = std::min(maxPlantMass, plants->mass);

			if (plants->wantsDeath)
			{
				delete plants;
				plants = 0;
			}
		}
		else
		{
			plantWantsChild = false;
		}

		if (animal)
		{
			if (animal->type == PopulationType::PREY)
			{
				if (plants)
				{
					if (plants->mass <= 0.2 * maxPlantMass)
						animal->Survey(false);
					else
						animal->Survey(true);
				}
				else
					animal->Survey(false);
			}

			//Update the animal
			animal->Update();

			if (animal->wantsEat)
			{
				if (animal->type == PopulationType::PREY)
				{
					if (plants)
					{
						float neededFood = (animal->stomachMax - animal->stomach) * 0.4f;
						if (plants->mass >= neededFood * 0.4f)
						{
							plants->mass -= neededFood * 0.4f;
							animal->Feed(neededFood);
						}
						else
						{
							animal->Feed(plants->mass);

							delete plants;
							plants = 0;
						}
					}
				}
				else
					predatorWantsFood = true;
			}

			animalWantsMove = animal->wantsMove;
			animalWantsChild = animal->wantsChild;

			if (animal->wantsDeath)
			{
				delete animal;
				animal = 0;
			}
		}
		else
		{
			animalWantsMove = false;
			animalWantsChild = false;
			predatorWantsFood = false;
		}
	}
	else
	{
		delete plants;
		plants = 0;

		delete animal;
		animal = 0;
	}
}