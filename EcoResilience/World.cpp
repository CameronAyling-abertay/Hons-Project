#include "World.h"
#include <random>
#include "CPerlinNoise/CPerlinNoise.h"
#include "NewPlant.h"

EcoResilience::World::World() :
	width(0),
	height(0),
	sunTime(0)
{}

EcoResilience::World::~World()
{
	Clear();
}

void EcoResilience::World::Generate(int w, int h, GenerationType type, float plantMassMax)
{
	Clear();

	width = w;
	height = h;

	maxCellPlantMass = plantMassMax;

	genType = type;

	//Generate the world
	//Random generation will randomly allocate cells water levels
	//Perlin generation will generate it using perlin noise for better natural-ness
	switch (genType)
	{
	case GenerationType::RANDOM:
		sunTime = float(rand() % 1000) / 24.f;

		for (int row = 0; row < height; row++)
		{
			for (int column = 0; column < width; column++)
			{
				Cell* newCell = new Cell(row, column, maxCellPlantMass);

				//Add water
				float water = float((rand() % 1000)) / 1000.f;
				newCell->SetWater(water);

				//Create plants
				float plantMass = ((rand() % 1000) / 1000.f) * maxCellPlantMass;

				int iterations = 0;
				while(iterations < 100)
				{
					Plant newPlant;
					newPlant.mass *= maxCellPlantMass;

					if (newPlant.mass > std::max(plantMass - maxCellPlantMass * 0.05f, 0.f) && newPlant.mass < std::min(plantMass + maxCellPlantMass * 0.05f, maxCellPlantMass))
					{
						newCell->AddPlant(newPlant);
						break;
					}

					iterations++;
				}

				//Add animals
				int animalPop = rand() % 10;
				for(int animalNum = 0; animalNum < animalPop; animalNum++)
					newCell->AddAnimal(EcoResilience::Animal());

				//Add the cell to the world vector
				push_back(newCell);
			}
		}
		break;

	case GenerationType::PERLIN:
		sunTime = float(rand() % 1000) / 24.f;
		const float waterOffset = rand();
		const float plantOffset = rand();
		const float animalOffset = rand();

		for (int row = 0; row < height; row++)
		{
			for (int column = 0; column < width; column++)
			{
				Cell* newCell = new Cell(row, column, maxCellPlantMass);

				//Create the water level
				float waterVec[2]{ (row + waterOffset) * 0.1f, (column + waterOffset) * 0.1f };
				float water = CPerlinNoise::noise2(waterVec) + 0.5f;

				newCell->SetWater(water);

				//Create the plants
				float plantVec[2]{ (row + plantOffset) * 0.1f, (column + plantOffset) * 0.1f };
				float plantMass = (CPerlinNoise::noise2(plantVec) + 0.5f) * maxCellPlantMass;

				int iterations = 0;
				while (iterations < 100)
				{
					Plant newPlant;
					newPlant.mass *= maxCellPlantMass;

					if (newPlant.mass > std::max(plantMass - maxCellPlantMass * 0.05f, 0.f) && newPlant.mass < std::min(plantMass + maxCellPlantMass * 0.05f, maxCellPlantMass))
					{
						newCell->AddPlant(newPlant);
						break;
					}

					iterations++;
				}

				//Create the animals
				float animalVec[2]{ (row + animalOffset) * 0.1f, (column + animalOffset) * 0.1f };
				int animalPop = (CPerlinNoise::noise2(animalVec) + 0.5f) * 10;

				for (int animalNum = 0; animalNum < animalPop; animalNum++)
					newCell->AddAnimal(Animal());

				//Add the cell to the world vector
				push_back(newCell);
			}
		}
	}

	steps = 0;
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
		data()[cellNum]->Update();

		//Handle child creation
		if (data()[cellNum]->desiredChildCount > 0)
		{
			for (int childNum = 0; childNum < data()[cellNum]->desiredChildCount; childNum++)
			{
				int iterations = 0;
				bool createdChild = false;
				while (iterations < 50 && !createdChild)
				{
					int dir = rand() % 5;

					EcoResilience::NewPlant newPlant;

					float currentMass;
					switch (dir)
					{

					case 0://Above this cell
						currentMass = 0;
						if (cellNum - width < 0)
							continue;

						for (auto plant : data()[cellNum - width]->plants)
							currentMass += plant.mass;

						if (currentMass + newPlant.mass < maxCellPlantMass)
						{
							data()[cellNum - width]->AddPlant(newPlant);
							createdChild = true;
						}
						break;

					case 1://Left of this cell
						currentMass = 0;
						if (cellNum % width == 0)
							continue;

						for (auto plant : data()[cellNum - 1]->plants)
							currentMass += plant.mass;

						if (currentMass + newPlant.mass < maxCellPlantMass)
						{
							data()[cellNum - 1]->AddPlant(newPlant);
							createdChild = true;
						}
						break;

					case 2://This cell
						currentMass = 0;
						for (auto plant : data()[cellNum]->plants)
							currentMass += plant.mass;

						if (currentMass + newPlant.mass < maxCellPlantMass)
						{
							data()[cellNum]->AddPlant(newPlant);
							createdChild = true;
						}
						break;

					case 3://Right of this cell
						currentMass = 0;
						if (cellNum % width == width - 1)
							continue;

						for (auto plant : data()[cellNum + 1]->plants)
							currentMass += plant.mass;

						if (currentMass + newPlant.mass < maxCellPlantMass)
						{
							data()[cellNum + 1]->AddPlant(newPlant);
							createdChild = true;
						}
						break;

					case 4://Below this cell
						currentMass = 0;
						if (cellNum + width > width * height - 1)
							continue;

						for (auto plant : data()[cellNum + width]->plants)
							currentMass += plant.mass;

						if (currentMass + newPlant.mass < maxCellPlantMass)
						{
							data()[cellNum + width]->AddPlant(newPlant);
							createdChild = true;
						}
						break;
					}

					iterations++;
				}
			}
		}

		data()[cellNum]->ResetChildCount();
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

void EcoResilience::World::Rain()
{
	switch (genType)
	{
	case GenerationType::RANDOM:
		for (int row = 0; row < height; row++)
		{
			for (int column = 0; column < width; column++)
			{
				float water = float((rand() % 1000)) / 1000.f;
				at(row * width + column)->SetWater(std::min(1.f, water + at(row * width + column)->GetWater()));
			}
		}
		break;

	case GenerationType::PERLIN:
		const float waterOffset = rand();

		for (int row = 0; row < height; row++)
		{
			for (int column = 0; column < width; column++)
			{
				float waterVec[2]{ (row + waterOffset) * 0.1f, (column + waterOffset) * 0.1f };
				float water = CPerlinNoise::noise2(waterVec) + 0.5f;

				at(row * width + column)->SetWater(std::min(1.f, water + at(row * width + column)->GetWater()));
			}
		}
	}
}