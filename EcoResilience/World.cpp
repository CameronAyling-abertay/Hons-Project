#include "World.h"
#include <random>
#include <set>
#include <vector>
#include "CPerlinNoise/CPerlinNoise.h"

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
				int preyPop = rand() % 10;
				if(preyPop <= 1)
					newCell->AddAnimal(Animal(PopulationType::PREY));

				int predatorPop = rand() % 10;
				if (predatorPop == 0)
					newCell->AddAnimal(Animal(PopulationType::PREDATOR));

				//Add the cell to the world vector
				push_back(newCell);
			}
		}
		break;

	case GenerationType::PERLIN:
		sunTime = float(rand() % 1000) / 24.f;
		const float waterOffset = rand();
		const float plantOffset = rand();
		const float preyOffset = rand();
		const float predatorOffset = rand();

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
				float preyVec[2]{ (row + preyOffset) * 0.1f, (column + preyOffset) * 0.1f };
				int preyPop = (CPerlinNoise::noise2(preyVec) + 0.5f) * 10;

				if(preyPop <= 1 || preyPop >= 9)
					newCell->AddAnimal(Animal(PopulationType::PREY));

				float predatorVec[2]{ (row + predatorOffset) * 0.1f, (column + predatorOffset) * 0.1f };
				int predatorPop = (CPerlinNoise::noise2(predatorVec) + 0.5f) * 10;

				if (predatorPop == 0 || predatorPop >= 9)
					newCell->AddAnimal(Animal(PopulationType::PREDATOR));

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
		//Fire Spread
		if (data()[cellNum]->plants)
		{
			if (data()[cellNum]->plants->fire)
				if (rand() % 100 < 20)
					data()[cellNum]->plants->Extinguish();

			if (data()[cellNum]->plants->fire)
			{
				//Up
				if (rand() % 100 < 80)
					if (cellNum >= width)
						if (data()[cellNum - width]->plants && data()[cellNum - width]->cellType == CellType::LAND)
							data()[cellNum - width]->plants->SetFire();

				//Left
				if (rand() % 100 < 80)
					if (cellNum % width > 0)
						if (data()[cellNum - 1]->plants && data()[cellNum - 1]->cellType == CellType::LAND)
							data()[cellNum - 1]->plants->SetFire();

				//Right
				if (rand() % 100 < 80)
					if (cellNum % width < width - 1)
						if (data()[cellNum + 1]->plants && data()[cellNum + 1]->cellType == CellType::LAND)
							data()[cellNum + 1]->plants->SetFire();

				//Down
				if (rand() % 100 < 80)
					if (cellNum < width * (height - 1))
						if (data()[cellNum + width]->plants && data()[cellNum + width]->cellType == CellType::LAND)
							data()[cellNum + width]->plants->SetFire();
			}
		}

		//Plague Spread


		//Plant child creation
		if (data()[cellNum]->plantWantsChild && data()[cellNum]->plants)
		{
			std::vector<int> ints{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
			std::vector<int> cellCheckOrder;
			cellCheckOrder.resize(12);

			for (int i = 0; i < 12; i++)
			{
				int pos = rand() % ints.size();
				cellCheckOrder[i] = ints[pos];
				ints.erase(ints.begin() + pos);
			}

			bool hadChild = false;
			while (!cellCheckOrder.empty() && !hadChild)
			{
				int place = cellCheckOrder.back();
				cellCheckOrder.pop_back();

				switch (place)
				{
				case 1:// Up 2
					if (cellNum >= width * 2)
					{
						if (!data()[cellNum - width * 2]->plants)
						{
							Plant newPlant(data()[cellNum]->plants->vigor * 0.1);
							data()[cellNum - width * 2]->AddPlant(newPlant);
							data()[cellNum]->plantHadChild();
							hadChild = true;
						}
					}
					break;

				case 2:// Up 1, Left 1
					if (cellNum >= width && cellNum % width > 0)
					{
						if (!data()[cellNum - width - 1]->plants)
						{
							Plant newPlant(data()[cellNum]->plants->vigor * 0.1);
							data()[cellNum - width - 1]->AddPlant(newPlant);
							data()[cellNum]->plantHadChild();
							hadChild = true;
						}
					}
					break;

				case 3:// Up 1
					if (cellNum >= width)
					{
						if (!data()[cellNum - width]->plants)
						{
							Plant newPlant(data()[cellNum]->plants->vigor * 0.1);
							data()[cellNum - width]->AddPlant(newPlant);
							data()[cellNum]->plantHadChild();
							hadChild = true;
						}
					}
					break;

				case 4:// Up 1, Right 1
					if (cellNum >= width && cellNum % width < width - 1)
					{
						if (!data()[cellNum - width + 1]->plants)
						{
							Plant newPlant(data()[cellNum]->plants->vigor * 0.1);
							data()[cellNum - width + 1]->AddPlant(newPlant);
							data()[cellNum]->plantHadChild();
							hadChild = true;
						}
					}
					break;

				case 5:// Left 2
					if (cellNum % width > 1)
					{
						if (!data()[cellNum - 2]->plants)
						{
							Plant newPlant(data()[cellNum]->plants->vigor * 0.1);
							data()[cellNum - 2]->AddPlant(newPlant);
							data()[cellNum]->plantHadChild();
							hadChild = true;
						}
					}
					break;

				case 6:// Left 1
					if (cellNum % width > 0)
					{
						if (!data()[cellNum - 1]->plants)
						{
							Plant newPlant(data()[cellNum]->plants->vigor * 0.1);
							data()[cellNum - 1]->AddPlant(newPlant);
							data()[cellNum]->plantHadChild();
							hadChild = true;
						}
					}
					break;

				case 7:// Right 1
					if (cellNum % width < width - 1)
					{
						if (!data()[cellNum + 1]->plants)
						{
							Plant newPlant(data()[cellNum]->plants->vigor * 0.1);
							data()[cellNum + 1]->AddPlant(newPlant);
							data()[cellNum]->plantHadChild();
							hadChild = true;
						}
					}
					break;

				case 8:// Right 2
					if (cellNum % width < width - 2)
					{
						if (!data()[cellNum + 2]->plants)
						{
							Plant newPlant(data()[cellNum]->plants->vigor * 0.1);
							data()[cellNum + 2]->AddPlant(newPlant);
							data()[cellNum]->plantHadChild();
							hadChild = true;
						}
					}
					break;

				case 9:// Down 1, Left 1
					if (cellNum < width * (height - 1) && cellNum % width > 0)
					{
						if (!data()[cellNum + width - 1]->plants)
						{
							Plant newPlant(data()[cellNum]->plants->vigor * 0.1);
							data()[cellNum + width - 1]->AddPlant(newPlant);
							data()[cellNum]->plantHadChild();
							hadChild = true;
						}
					}
					break;

				case 10:// Down 1
					if (cellNum < width * (height - 1))
					{
						if (!data()[cellNum + width]->plants)
						{
							Plant newPlant(data()[cellNum]->plants->vigor * 0.1);
							data()[cellNum + width]->AddPlant(newPlant);
							data()[cellNum]->plantHadChild();
							hadChild = true;
						}
					}
					break;

				case 11:// Down 1, Right 1
					if (cellNum < width * (height - 1) && cellNum % width < width - 1)
					{
						if (!data()[cellNum + width + 1]->plants)
						{
							Plant newPlant(data()[cellNum]->plants->vigor * 0.1);
							data()[cellNum + width + 1]->AddPlant(newPlant);
							data()[cellNum]->plantHadChild();
							hadChild = true;
						}
					}
					break;

				case 12:// Down 2
					if (cellNum + width < width * (height - 1))
					{
						if (!data()[cellNum + width * 2]->plants)
						{
							Plant newPlant(data()[cellNum]->plants->vigor * 0.1);
							data()[cellNum + width * 2]->AddPlant(newPlant);
							data()[cellNum]->plantHadChild();
							hadChild = true;
						}
					}
					break;
				}
			}
		}

		data()[cellNum]->ResetChildCount();

		//Animal Movement
		if (data()[cellNum]->animalWantsMove && data()[cellNum]->animal)
		{
			std::vector<std::pair<float, int>> masses;
			masses.resize(4);
			for (int i = 0; i < 4; i++)
				masses[i] = std::pair<float, int>(0, i + 1);

			if (cellNum >= width)
				if (data()[cellNum - width]->plants)
					masses[0].first = data()[cellNum - width]->plants->mass;

			if (cellNum % width > 0)
				if (data()[cellNum - 1]->plants)
					masses[1].first = data()[cellNum - 1]->plants->mass;

			if (cellNum % width < width - 1)
				if (data()[cellNum + 1]->plants)
					masses[2].first = data()[cellNum + 1]->plants->mass;

			if (cellNum < width * (height - 1))
				if (data()[cellNum + width]->plants)
					masses[3].first = data()[cellNum + width]->plants->mass;

			for (int i = 1; i < masses.size(); i++)
			{
				std::pair<float, int> key = masses[i];
				int j = i - 1;

				while (j >= 0)
				{
					if (key.first < masses[j].first)
					{
						masses[j + 1] = masses[j];
						j--;
					}
					else
						break;
				}

				masses[j + 1] = key;
			}

			bool moved = false;
			while (!moved && !masses.empty())
			{
				switch (masses.back().second)
				{
				case 1://Up 1
					if (cellNum >= width)
					{
						if (!data()[cellNum - width]->animal && data()[cellNum - width]->cellType == CellType::LAND)
						{
							data()[cellNum - width]->AddAnimal(*data()[cellNum]->animal);
							data()[cellNum]->animalMoved();
							moved = true;
						}
					}

					break;

				case 2://Left 1
					if (cellNum % width > 0)
					{
						if (!data()[cellNum - 1]->animal && data()[cellNum - 1]->cellType == CellType::LAND)
						{
							data()[cellNum - 1]->AddAnimal(*data()[cellNum]->animal);
							data()[cellNum]->animalMoved();
							moved = true;
						}
					}

					break;

				case 3://Right 1
					if (cellNum % width < width - 1)
					{
						if (!data()[cellNum + 1]->animal && data()[cellNum + 1]->cellType == CellType::LAND)
						{
							data()[cellNum + 1]->AddAnimal(*data()[cellNum]->animal);
							data()[cellNum]->animalMoved();
							moved = true;
						}
					}

					break;

				case 4://Down 1
					if (cellNum < width * (height - 1))
					{
						if (!data()[cellNum + width]->animal && data()[cellNum + width]->cellType == CellType::LAND)
						{
							data()[cellNum + width]->AddAnimal(*data()[cellNum]->animal);
							data()[cellNum]->animalMoved();
							moved = true;
						}
					}

					break;
				}

				masses.pop_back();
			}
		}

		//Predator Eating
		if(data()[cellNum]->predatorWantsFood && data()[cellNum]->animal)
		{
			bool preyPresence[4]{ false };
			int preyNum = 0;

			if (cellNum >= width)
				if (data()[cellNum - width]->animal)
					if(data()[cellNum - width]->animal->type == PopulationType::PREY)
						preyPresence[0] = true;

			if (cellNum % width > 0)
				if (data()[cellNum - 1]->animal)
					if (data()[cellNum - 1]->animal->type == PopulationType::PREY)
						preyPresence[1] = true;

			if (cellNum % width < width - 1)
				if (data()[cellNum + 1]->animal)
					if (data()[cellNum + 1]->animal->type == PopulationType::PREY)
						preyPresence[2] = true;

			if (cellNum < width * (height - 1))
				if (data()[cellNum + width]->animal)
					if (data()[cellNum + width]->animal->type == PopulationType::PREY)
						preyPresence[3] = true;

			for (auto cell : preyPresence)
				if (cell)
					preyNum++;

			if (preyNum > 0)
			{
				int preyDir = rand() % preyNum + 1;

				int preyIndex = -1;
				while (preyDir > 0)
				{
					preyIndex++;
					if (preyPresence[preyIndex])
					{
						preyDir--;
					}
				}

				switch(preyIndex)
				{
				case 0:
					data()[cellNum]->feedPredator(data()[cellNum - width]->animal->mass);
					data()[cellNum - width]->killPrey();
					break;

				case 1:
					data()[cellNum]->feedPredator(data()[cellNum - 1]->animal->mass);
					data()[cellNum - 1]->killPrey();
					break;

				case 2:
					data()[cellNum]->feedPredator(data()[cellNum + 1]->animal->mass);
					data()[cellNum + 1]->killPrey();
					break;

				case 3:
					data()[cellNum]->feedPredator(data()[cellNum + width]->animal->mass);
					data()[cellNum + width]->killPrey();
					break;
				}
			}
		}

		data()[cellNum]->Update();
	}

	//Animal child creation
	for(int cellNum = 0; cellNum < width * height; cellNum++)
	{
		if (data()[cellNum]->animalWantsChild && data()[cellNum]->animal)
		{
			PopulationType childType = data()[cellNum]->animal->type;

			bool childBearers[4]{ false };
			int bearerNum = 0;

			if (cellNum >= width)
				if (data()[cellNum - width]->animalWantsChild && data()[cellNum - width]->animal)
					if(data()[cellNum - width]->animal->type == childType)
						childBearers[0] = true;

			if (cellNum % width > 0)
				if (data()[cellNum - 1]->animalWantsChild && data()[cellNum - 1]->animal)
					if (data()[cellNum - 1]->animal->type == childType)
						childBearers[1] = true;

			if (cellNum % width < width - 1)
				if (data()[cellNum + 1]->animalWantsChild && data()[cellNum + 1]->animal)
					if (data()[cellNum + 1]->animal->type == childType)
						childBearers[2] = true;

			if (cellNum < width * (height - 1))
				if (data()[cellNum + width]->animalWantsChild && data()[cellNum + width]->animal)
					if (data()[cellNum + width]->animal->type == childType)
						childBearers[3] = true;

			for (auto cell : childBearers)
				if (cell)
					bearerNum++;

			if(bearerNum > 0)
			{
				int bearerDir = rand() % bearerNum + 1;

				int bearerIndex = -1;
				while(bearerDir > 0)
				{
					bearerIndex++;
					if (childBearers[bearerIndex])
					{
						bearerDir--;
					}
				}

				bool bearer = rand() % 2;

				switch(bearerIndex)
				{
				case 0://Up
					if(bearer)//Children spawn next to this cell
					{
						if (cellNum % width > 0)
							if(!data()[cellNum - 1]->animal && data()[cellNum - 1]->cellType == CellType::LAND)
								data()[cellNum - 1]->AddAnimal(Animal(childType));

						if (cellNum % width < width - 1)
							if (!data()[cellNum + 1]->animal && data()[cellNum + 1]->cellType == CellType::LAND)
								data()[cellNum + 1]->AddAnimal(Animal(childType));
					}
					else
					{
						if (cellNum % width > 0)
							if (!data()[cellNum - width - 1]->animal && data()[cellNum - width - 1]->cellType == CellType::LAND)
								data()[cellNum - width - 1]->AddAnimal(Animal(childType));

						if (cellNum % width < width - 1)
							if (!data()[cellNum - width + 1]->animal && data()[cellNum - width + 1]->cellType == CellType::LAND)
								data()[cellNum - width + 1]->AddAnimal(Animal(childType));
					}

					break;

				case 1://Left
					if (bearer)//Children spawn next to this cell
					{
						if (cellNum >= width)
							if (!data()[cellNum - width]->animal && data()[cellNum - width]->cellType == CellType::LAND)
								data()[cellNum - width]->AddAnimal(Animal(childType));

						if (cellNum + width < width * (height - 1))
							if (!data()[cellNum + width]->animal && data()[cellNum + width]->cellType == CellType::LAND)
								data()[cellNum + width]->AddAnimal(Animal(childType));
					}
					else
					{
						if (cellNum >= width)
							if (!data()[cellNum - 1 - width]->animal && data()[cellNum - 1 - width]->cellType == CellType::LAND)
								data()[cellNum - 1 - width]->AddAnimal(Animal(childType));

						if (cellNum + width < width * (height - 1))
							if (!data()[cellNum - 1 + width]->animal && data()[cellNum - 1 + width]->cellType == CellType::LAND)
								data()[cellNum - 1 + width]->AddAnimal(Animal(childType));
					}

					break;

				case 2://Right
					if (bearer)//Children spawn next to this cell
					{
						if (cellNum >= width)
							if (!data()[cellNum - width]->animal && data()[cellNum - width]->cellType == CellType::LAND)
								data()[cellNum - width]->AddAnimal(Animal(childType));

						if (cellNum + width < width * (height - 1))
							if (!data()[cellNum + width]->animal && data()[cellNum + width]->cellType == CellType::LAND)
								data()[cellNum + width]->AddAnimal(Animal(childType));
					}
					else
					{
						if (cellNum >= width)
							if (!data()[cellNum + 1 - width]->animal && data()[cellNum + 1 - width]->cellType == CellType::LAND)
								data()[cellNum + 1 - width]->AddAnimal(Animal(childType));

						if (cellNum + width < width * (height - 1))
							if (!data()[cellNum + 1 + width]->animal && data()[cellNum + 1 + width]->cellType == CellType::LAND)
								data()[cellNum + 1 + width]->AddAnimal(Animal(childType));
					}

					break;

				case 3://Down
					if (bearer)//Children spawn next to this cell
					{
						if (cellNum % width > 0)
							if (!data()[cellNum - 1]->animal && data()[cellNum - 1]->cellType == CellType::LAND)
								data()[cellNum - 1]->AddAnimal(Animal(childType));

						if (cellNum % width < width - 1)
							if (!data()[cellNum + 1]->animal && data()[cellNum + 1]->cellType == CellType::LAND)
								data()[cellNum + 1]->AddAnimal(Animal(childType));
					}
					else
					{
						if (cellNum % width > 0)
							if (!data()[cellNum + width - 1]->animal && data()[cellNum + width - 1]->cellType == CellType::LAND)
								data()[cellNum + width - 1]->AddAnimal(Animal(childType));

						if (cellNum % width < width - 1)
							if (!data()[cellNum + width + 1]->animal && data()[cellNum + width + 1]->cellType == CellType::LAND)
								data()[cellNum + width + 1]->AddAnimal(Animal(childType));
					}

					break;
				}

				data()[cellNum]->animalHadChild();
			}
		}
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
				float water = (CPerlinNoise::noise2(waterVec) + 0.5f) * 0.4;

				at(row * width + column)->SetWater(std::min(1.f, water + at(row * width + column)->GetWater()));
			}
		}
	}
}

void EcoResilience::World::UrbanDevelop()
{
	for(int cellNum = 0; cellNum < width * height; cellNum++)
	{
		if(cellNum % width < width / 3)
		{
			data()[cellNum]->cellType = CellType::URBANISED;
		}
	}
}
