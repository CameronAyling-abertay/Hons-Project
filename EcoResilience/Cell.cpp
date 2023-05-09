#include "Cell.h"

EcoResilience::Cell::Cell(int row, int column, float plantMass) :
	waterLevel(0.5),
	cellRow(row),
	cellColumn(column),
	plants(NULL),
	plantWantsChild(false),
	animalWantsMove(false),
	maxMass(plantMass),
	flooded(false),
	hasPlant(false),
	hasAnimal(false),
	fire(false)
{
}

//A diagnostic/species detection function to check if the cell is inhabited by an animal and by what species at the same time
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

//Getters and Setters
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
		if (waterLevel > altitude)
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

//Update
void EcoResilience::Cell::Update()
{
	//If the cell is flooded, drain it a bit
	if(flooded)
	{
		SetWater(waterLevel * 0.99);
		floodCounter++;
		if (floodCounter > 50)
		{
			flooded = false;
			floodCounter = 0;
		}
	}

	//Manage whether the cell is on fire
	if (hasPlant)
	{
		fire = plants.fire;
	}
	else
		fire = false;

	if(hasAnimal)
	{
		if (fire)
			animal.Burn();
		else
			animal.RecoverBurn();
	}

	//Different actions based on what the landscape is like within the cell
	if (cellType == CellType::WATER)//The landscape is underwater
	{
		//Slightly drain the cell to simulate evaporation
		SetWater(waterLevel * 0.99995f);

		//Drown the biota in the cell
		if (hasAnimal)
			animal.Drown();

		if (hasPlant)
			plants.Drown();
	}
	else if (cellType == CellType::LAND)//The landscape is inhabitable
	{
		//Manage the plants
		if (hasPlant)
		{
			//Because the cell is not water, allow the cell to recover from drowning if it was previously
			plants.Resuscitate();

			//If the plant is hungry so give it some food and drain the appropriate amount of water
			if (plants.wantsFood)
			{
				float mod = -0.5 + rand() % 1000 / 1000.f;

				if (waterLevel - plants.GetNeededFood() * (0.005 + 0.005 * mod) > 0)
				{
					plants.Feed(plants.GetNeededFood() * (0.6 + 0.2 * mod));
					SetWater(waterLevel - plants.GetNeededFood() * (0.005 + 0.005 * mod));
				}
			}

			//Check whether the plant wants a child
			plantWantsChild = plants.wantsChild;

			//Cap the plant's mass
			plants.CapMass(maxMass);
		}
		else//There is no plant so reset related flags
		{
			plantWantsChild = false;
		}

		//Manage the animal
		if (hasAnimal)
		{
			//Because the cell is not water, allow the cell to recover from drowning if it was previously
			animal.Resuscitate();

			//If the animal is hungry, do different things depending on the animal's species
			if (animal.wantsEat)
			{
				//If the animal is a prey animal, give it some plant mass and subtract the appropriate amount from the plants in the cell
				if (animal.type == PopulationType::PREY)
				{
					if (hasPlant)
					{
						//Take as much plant mass as is available up to a cap
						float neededFood = animal.GetNeededFood() * 0.7f;
						if (plants.GetMass() >= neededFood * 0.3f)
						{
							plants.Decay(neededFood * 0.3f);
							animal.Feed(neededFood);
						}
						else//Otherwise take everything and kill the plants
						{
							animal.Feed(plants.GetMass());
							hasPlant = false;
						}
					}
				}
				else//The animal is a predator so it needs to be handled by the world object
					predatorWantsFood = true;
			}

			//Set flags
			animalWantsMove = animal.wantsMove;
			animalWantsChild = animal.wantsChild;

			//Cap the mass
			animal.CapMass(maxMass);
		}
		else
		{
			//There is no animal so reset flags
			animalWantsMove = false;
			animalWantsChild = false;
			predatorWantsFood = false;
		}
	}
	else
	{
		//The cell is urbanised so kill all biota
		hasAnimal = false;
		hasPlant = false;
	}

	//Update the animal
	if (hasAnimal)
	{
		animal.Update();

		//If the animal should die, kill it
		if (animal.wantsDeath)
			hasAnimal = false;
	}

	//Update the plant
	if (hasPlant)
	{
		plants.Update();

		//If the plants should die, kill them
		if (plants.wantsDeath)
			hasPlant = false;
	}
}