#include "Cell.h"

EcoResilience::Cell::Cell(int row, int column, float plantMass) :
	waterLevel(0.5),
	cellRow(row),
	cellColumn(column),
	plants(NULL),
	plantWantsChild(false),
	animalWantsMove(false),
	maxPlantMass(plantMass),
	flooded(false),
	hasPlant(false),
	hasAnimal(false)
{
}

int EcoResilience::Cell::GetPopulation(PopulationType type)
{
	if (hasAnimal)
	{
		switch (type)
		{
		case PopulationType::PREY:
			if (animal.type == PopulationType::PREY)
				return 1;
			return 0;

		case PopulationType::PREDATOR:
			if (animal.type == PopulationType::PREDATOR)
				return 1;
			return 0;
		}
	}
	return 0;
}

void EcoResilience::Cell::AddPlant(Plant plant)
{
	plants = plant;
	hasPlant = true;
}

void EcoResilience::Cell::AddAnimal(Animal fauna)
{
	animal = fauna;
	hasAnimal = true;
}

void EcoResilience::Cell::SetWater(float newWaterLevel)
{
	if (!(cellType == CellType::URBANISED))
	{
		waterLevel = std::min(1.f, std::max(0.f, newWaterLevel));
		if (waterLevel * 255.f > 150.f)
		{
			cellType = CellType::WATER;
			if (hasPlant)
				if (plants.fire)
					plants.Extinguish();
		}
		else
			cellType = CellType::LAND;
	}
}

void EcoResilience::Cell::Update()
{
	if(flooded)
	{
		SetWater(waterLevel * 0.99);
		floodCounter++;
		if (floodCounter > 50)
			flooded = false;
	}

	if (cellType == CellType::WATER)
	{
		hasAnimal = false;
	}
	else if (cellType == CellType::LAND)
	{
		if (hasPlant)
		{
			plants.Update();

			if (plants.wantsFood)
			{
				float mod = -0.5 + rand() % 1000 / 1000.f;

				if (waterLevel - plants.stomachMax * (0.02 + 0.02 * mod) > 0)
				{
					plants.Feed(plants.stomachMax * (0.6 + 0.2 * mod));
					SetWater(waterLevel - plants.stomachMax * (0.02 + 0.02 * mod));
				}
			}

			plantWantsChild = plants.wantsChild;

			plants.mass = std::min(maxPlantMass, plants.mass);

			if (plants.wantsDeath)
			{
				hasPlant = false;
			}
		}
		else
		{
			plantWantsChild = false;
		}

		if (hasAnimal)
		{
			if (animal.type == PopulationType::PREY)
			{
				if (hasPlant)
				{
					if (plants.mass <= 0.2 * maxPlantMass)
						animal.Survey(false);
					else
						animal.Survey(true);
				}
				else
					animal.Survey(false);
			}

			//Update the animal
			animal.Update();

			if (animal.wantsEat)
			{
				if (animal.type == PopulationType::PREY)
				{
					if (hasPlant)
					{
						float neededFood = (animal.stomachMax - animal.stomach) * 0.4f;
						if (plants.mass >= neededFood * 0.4f)
						{
							plants.mass -= neededFood * 0.4f;
							animal.Feed(neededFood);
						}
						else
						{
							animal.Feed(plants.mass);

							hasPlant = false;
						}
					}
				}
				else
					predatorWantsFood = true;
			}

			animalWantsMove = animal.wantsMove;
			animalWantsChild = animal.wantsChild;

			if (animal.wantsDeath)
			{
				hasAnimal = false;
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
		waterLevel = 0;

		hasAnimal = false;

		hasPlant = false;
	}
}