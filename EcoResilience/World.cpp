#include "World.h"
#include <random>
#include <vector>
#include "CPerlinNoise/CPerlinNoise.h"

EcoResilience::World::World() :
	width(0),
	height(0)
{}

void EcoResilience::World::Generate(int w, int h, GenerationType type, float plantMassMax)
{
	clear();

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
		for (int row = 0; row < height; row++)
		{
			for (int column = 0; column < width; column++)
			{
				Cell newCell(row, column, maxCellPlantMass);

				//Add water
				float water = float((rand() % 1000)) / 1000.f;
				newCell.SetWater(water);

				//Set Altitude
				float altitude = float(rand() % 1000 / 1000.f) * 0.55 + 0.45;
				newCell.SetAltitude(altitude);

				//Create plants
				float plantMass = ((rand() % 1000) / 1000.f) * maxCellPlantMass;

				int iterations = 0;
				while(iterations < 100)
				{
					Plant newPlant;
					newPlant.TopMass(maxCellPlantMass);

					if (newPlant.GetMass() > std::max(plantMass - maxCellPlantMass * 0.05f, 0.f) && newPlant.GetMass() < std::min(plantMass + maxCellPlantMass * 0.05f, maxCellPlantMass))
					{
						newCell.AddPlant(newPlant);
						break;
					}

					iterations++;
				}

				//Add animals
				int preyPop = rand() % 10;
				if(preyPop <= 1)
					newCell.AddAnimal(Animal(PopulationType::PREY));

				int predatorPop = rand() % 10;
				if (predatorPop == 0)
					newCell.AddAnimal(Animal(PopulationType::PREDATOR));

				//Add the cell to the world vector
				push_back(newCell);
			}
		}
		break;

	case GenerationType::PERLIN:
		const float waterOffset = rand();
		const float altOffset = rand();
		const float plantOffset = rand();
		const float preyOffset = rand();
		const float predatorOffset = rand();

		for (int row = 0; row < height; row++)
		{
			for (int column = 0; column < width; column++)
			{
				Cell newCell(row, column, maxCellPlantMass);

				//Create the water level
				float waterVec[2]{ (row + waterOffset) * 0.1f, (column + waterOffset) * 0.1f };
				float water = CPerlinNoise::noise2(waterVec) + 0.5f;

				newCell.SetWater(water);

				//Set altitude
				float altVec[2]{ (row + altOffset) * 0.1f, (column + altOffset) * 0.1f };
				float altitude = (CPerlinNoise::noise2(altVec) + 0.5f) * 0.55 + 0.45;

				newCell.SetAltitude(altitude);

				//Create the plants
				float plantVec[2]{ (row + plantOffset) * 0.1f, (column + plantOffset) * 0.1f };
				float plantMass = (CPerlinNoise::noise2(plantVec) + 0.5f) * maxCellPlantMass;

				int iterations = 0;
				while (iterations < 100)
				{
					Plant newPlant;
					newPlant.TopMass(maxCellPlantMass);

					if (newPlant.GetMass() > std::max(plantMass - maxCellPlantMass * 0.05f, 0.f) && newPlant.GetMass() < std::min(plantMass + maxCellPlantMass * 0.05f, maxCellPlantMass))
					{
						newCell.AddPlant(newPlant);
						break;
					}

					iterations++;
				}

				//Create the animals
				float preyVec[2]{ (row + preyOffset) * 0.1f, (column + preyOffset) * 0.1f };
				int preyPop = (CPerlinNoise::noise2(preyVec) + 0.5f) * 10;

				if(preyPop <= 1 || preyPop >= 9)
					newCell.AddAnimal(Animal(PopulationType::PREY));

				float predatorVec[2]{ (row + predatorOffset) * 0.1f, (column + predatorOffset) * 0.1f };
				int predatorPop = (CPerlinNoise::noise2(predatorVec) + 0.5f) * 20;

				if (predatorPop == 0)
					newCell.AddAnimal(Animal(PopulationType::PREDATOR));

				//Add the cell to the world vector
				push_back(newCell);
			}
		}
	}

	steps = 0;
}

EcoResilience::World EcoResilience::World::Update()
{
	World newWorld = *this;

	for (int cellNum = 0; cellNum < width * height; cellNum++)
	{
		Cell currentCell = at(cellNum);

		//Fire Spread
		if (currentCell.hasPlant)
		{
			if (currentCell.GetPlant().fire)
			{
				//Up
				if (rand() % 100 < 80)
					if (cellNum >= width)
						if (at(cellNum - width).hasPlant && at(cellNum - width).cellType == CellType::LAND)
							newWorld[cellNum - width].SetFire();

				//Left
				if (rand() % 100 < 80)
					if (cellNum % width > 0)
						if (at(cellNum - 1).hasPlant && at(cellNum - 1).cellType == CellType::LAND)
							newWorld[cellNum - 1].SetFire();

				//Right
				if (rand() % 100 < 80)
					if (cellNum % width < width - 1)
						if (at(cellNum + 1).hasPlant && at(cellNum + 1).cellType == CellType::LAND)
							newWorld[cellNum + 1].SetFire();

				//Down
				if (rand() % 100 < 80)
					if (cellNum < width * (height - 1))
						if (at(cellNum + width).hasPlant && at(cellNum + width).cellType == CellType::LAND)
							newWorld[cellNum + width].SetFire();
			}
		}

		//Plague Spread
		if (currentCell.hasAnimal)
		{
			if (currentCell.GetAnimal().infected)
			{
				//Up
				if (rand() % 100 < 80)
					if (cellNum >= width)
						if (at(cellNum - width).hasAnimal)
							newWorld[cellNum - width].InfectAnimal();

				//Left
				if (rand() % 100 < 80)
					if (cellNum % width > 0)
						if (at(cellNum - 1).hasAnimal)
							newWorld[cellNum - 1].InfectAnimal();

				//Right
				if (rand() % 100 < 80)
					if (cellNum % width < width - 1)
						if (at(cellNum + 1).hasAnimal)
							newWorld[cellNum + 1].InfectAnimal();

				//Down
				if (rand() % 100 < 80)
					if (cellNum < width * (height - 1))
						if (at(cellNum + width).hasAnimal)
							newWorld[cellNum + width].InfectAnimal();
			}
		}

		//Plant child creation
		if (currentCell.plantWantsChild && currentCell.hasPlant)
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
						if (!at(cellNum - width * 2).hasPlant)
						{
							Plant newPlant(currentCell.GetPlant().GetVigor() * 0.1);
							newWorld[cellNum - width * 2].AddPlant(newPlant);
							newWorld[cellNum].plantHadChild();
							hadChild = true;
						}
					}
					break;

				case 2:// Up 1, Left 1
					if (cellNum >= width && cellNum % width > 0)
					{
						if (!at(cellNum - width - 1).hasPlant)
						{
							Plant newPlant(currentCell.GetPlant().GetVigor() * 0.1);
							newWorld[cellNum - width - 1].AddPlant(newPlant);
							newWorld[cellNum].plantHadChild();
							hadChild = true;
						}
					}
					break;

				case 3:// Up 1
					if (cellNum >= width)
					{
						if (!at(cellNum - width).hasPlant)
						{
							Plant newPlant(currentCell.GetPlant().GetVigor() * 0.1);
							newWorld[cellNum - width].AddPlant(newPlant);
							newWorld[cellNum].plantHadChild();
							hadChild = true;
						}
					}
					break;

				case 4:// Up 1, Right 1
					if (cellNum >= width && cellNum % width < width - 1)
					{
						if (!at(cellNum - width + 1).hasPlant)
						{
							Plant newPlant(currentCell.GetPlant().GetVigor() * 0.1);
							newWorld[cellNum - width + 1].AddPlant(newPlant);
							newWorld[cellNum].plantHadChild();
							hadChild = true;
						}
					}
					break;

				case 5:// Left 2
					if (cellNum % width > 1)
					{
						if (!at(cellNum - 2).hasPlant)
						{
							Plant newPlant(currentCell.GetPlant().GetVigor() * 0.1);
							newWorld[cellNum - 2].AddPlant(newPlant);
							newWorld[cellNum].plantHadChild();
							hadChild = true;
						}
					}
					break;

				case 6:// Left 1
					if (cellNum % width > 0)
					{
						if (!at(cellNum - 1).hasPlant)
						{
							Plant newPlant(currentCell.GetPlant().GetVigor() * 0.1);
							newWorld[cellNum - 1].AddPlant(newPlant);
							newWorld[cellNum].plantHadChild();
							hadChild = true;
						}
					}
					break;

				case 7:// Right 1
					if (cellNum % width < width - 1)
					{
						if (!at(cellNum + 1).hasPlant)
						{
							Plant newPlant(currentCell.GetPlant().GetVigor() * 0.1);
							newWorld[cellNum + 1].AddPlant(newPlant);
							newWorld[cellNum].plantHadChild();
							hadChild = true;
						}
					}
					break;

				case 8:// Right 2
					if (cellNum % width < width - 2)
					{
						if (!at(cellNum + 2).hasPlant)
						{
							Plant newPlant(currentCell.GetPlant().GetVigor() * 0.1);
							newWorld[cellNum + 2].AddPlant(newPlant);
							newWorld[cellNum].plantHadChild();
							hadChild = true;
						}
					}
					break;

				case 9:// Down 1, Left 1
					if (cellNum < width * (height - 1) && cellNum % width > 0)
					{
						if (!at(cellNum + width - 1).hasPlant)
						{
							Plant newPlant(currentCell.GetPlant().GetVigor() * 0.1);
							newWorld[cellNum + width - 1].AddPlant(newPlant);
							newWorld[cellNum].plantHadChild();
							hadChild = true;
						}
					}
					break;

				case 10:// Down 1
					if (cellNum < width * (height - 1))
					{
						if (!at(cellNum + width).hasPlant)
						{
							Plant newPlant(currentCell.GetPlant().GetVigor() * 0.1);
							newWorld[cellNum + width].AddPlant(newPlant);
							newWorld[cellNum].plantHadChild();
							hadChild = true;
						}
					}
					break;

				case 11:// Down 1, Right 1
					if (cellNum < width * (height - 1) && cellNum % width < width - 1)
					{
						if (!at(cellNum + width + 1).hasPlant)
						{
							Plant newPlant(currentCell.GetPlant().GetVigor() * 0.1);
							newWorld[cellNum + width + 1].AddPlant(newPlant);
							newWorld[cellNum].plantHadChild();
							hadChild = true;
						}
					}
					break;

				case 12:// Down 2
					if (cellNum + width < width * (height - 1))
					{
						if (!at(cellNum + width * 2).hasPlant)
						{
							Plant newPlant(currentCell.GetPlant().GetVigor() * 0.1);
							newWorld[cellNum + width * 2].AddPlant(newPlant);
							newWorld[cellNum].plantHadChild();
							hadChild = true;
						}
					}
					break;
				}
			}
		}

		//Animal Movement
		if (currentCell.animalWantsMove && currentCell.hasAnimal)
		{
			//Weigh directions
			//Form weight directory
			std::vector<std::pair<float, int>> weights;
			for (int i = 0; i < 5; i++)
				weights.push_back(std::pair<float, int>(0, i));

			//Create the weights
			//Prey weights
			if (currentCell.GetAnimal().type == PopulationType::PREY)
			{
				float prio = currentCell.GetAnimal().GetNeededFood();

				//Plant mass
				{
					//Up 2
					if (currentCell.cellRow >= 2)
						if (at(cellNum - width * 2).hasPlant)
							weights[1].first += at(cellNum - width * 2).GetPlant().GetMass() * prio;

					//Up 1, Left 1
					if (currentCell.cellRow >= 1 && currentCell.cellColumn >= 1)
						if (at(cellNum - width - 1).hasPlant)
						{
							weights[1].first += at(cellNum - width - 1).GetPlant().GetMass() * prio;
							weights[2].first += at(cellNum - width - 1).GetPlant().GetMass() * prio;
						}

					//Up 1
					if (currentCell.cellRow >= 1)
						if (at(cellNum - width).hasPlant)
							weights[1].first += at(cellNum - width).GetPlant().GetMass() * prio;

					//Up 1, Right 1
					if (currentCell.cellRow >= 1 && currentCell.cellColumn < width - 1)
						if (at(cellNum - width + 1).hasPlant)
						{
							weights[1].first += at(cellNum - width + 1).GetPlant().GetMass() * prio;
							weights[3].first += at(cellNum - width + 1).GetPlant().GetMass() * prio;
						}

					//Left 2
					if (currentCell.cellColumn >= 2)
						if (at(cellNum - 2).hasPlant)
							weights[2].first += at(cellNum - 2).GetPlant().GetMass() * prio;

					//Left 1
					if (currentCell.cellColumn >= 1)
						if (at(cellNum - 1).hasPlant)
							weights[2].first += at(cellNum - 1).GetPlant().GetMass() * prio;

					//This cell
					if (currentCell.hasPlant)
						weights[0].first += currentCell.GetPlant().GetMass() * prio;

					//Right 1
					if (currentCell.cellColumn < width - 1)
						if (at(cellNum + 1).hasPlant)
							weights[3].first += at(cellNum + 1).GetPlant().GetMass() * prio;

					//Right 2
					if (currentCell.cellColumn < width - 2)
						if (at(cellNum + 2).hasPlant)
							weights[3].first += at(cellNum + 2).GetPlant().GetMass() * prio;

					//Down 1, Left 1
					if (currentCell.cellRow < height - 1 && currentCell.cellColumn >= 1)
						if (at(cellNum + width - 1).hasPlant)
						{
							weights[2].first += at(cellNum + width - 1).GetPlant().GetMass() * prio;
							weights[4].first += at(cellNum + width - 1).GetPlant().GetMass() * prio;
						}

					//Down 1
					if (currentCell.cellRow < height - 1)
						if (at(cellNum + width).hasPlant)
							weights[4].first += at(cellNum + width).GetPlant().GetMass() * prio;

					//Down 1, Right 1
					if (currentCell.cellRow < height - 1 && currentCell.cellColumn < width - 1)
						if (at(cellNum + width + 1).hasPlant)
						{
							weights[3].first += at(cellNum + width + 1).GetPlant().GetMass() * prio;
							weights[4].first += at(cellNum + width + 1).GetPlant().GetMass() * prio;
						}

					//Down 2
					if (currentCell.cellRow < height - 2)
						if (at(cellNum + width * 2).hasPlant)
							weights[4].first += at(cellNum + width * 2).GetPlant().GetMass() * prio;
				}

				//Predator presence
				{
					//Up 2
					if (currentCell.cellRow >= 2)
						if (at(cellNum - width * 2).GetPopulation(PopulationType::PREDATOR))
						{
							weights[0].first -= 1;
							weights[1].first -= 2;
							weights[4].first += 2;
						}

					//Up 1, Left 1
					if (currentCell.cellRow >= 1 && currentCell.cellColumn >= 1)
						if (at(cellNum - width - 1).GetPopulation(PopulationType::PREDATOR))
						{
							weights[0].first -= 1;
							weights[1].first -= 2;
							weights[2].first -= 2;
							weights[3].first += 2;
							weights[4].first += 2;
						}

					//Up 1
					if (currentCell.cellRow >= 1)
						if (at(cellNum - width).GetPopulation(PopulationType::PREDATOR))
						{
							weights[0].first -= 1;
							weights[1].first -= 2;
							weights[4].first += 2;
						}

					//Up 1, Right 1
					if (currentCell.cellRow >= 1 && currentCell.cellColumn < width - 1)
						if (at(cellNum - width + 1).GetPopulation(PopulationType::PREDATOR))
						{
							weights[0].first -= 1;
							weights[1].first -= 2;
							weights[3].first -= 2;
							weights[2].first += 2;
							weights[4].first += 2;
						}

					//Left 2
					if (currentCell.cellColumn >= 2)
						if (at(cellNum - 2).GetPopulation(PopulationType::PREDATOR))
						{
							weights[0].first -= 1;
							weights[2].first -= 2;
							weights[3].first += 2;
						}

					//Left 1
					if (currentCell.cellColumn >= 1)
						if (at(cellNum - 1).GetPopulation(PopulationType::PREDATOR))
						{
							weights[0].first -= 1;
							weights[2].first -= 2;
							weights[3].first += 2;
						}

					//Right 1
					if (currentCell.cellColumn < width - 1)
						if (at(cellNum + 1).GetPopulation(PopulationType::PREDATOR))
						{
							weights[0].first -= 1;
							weights[3].first -= 2;
							weights[2].first += 2;
						}

					//Right 2
					if (currentCell.cellColumn < width - 2)
						if (at(cellNum + 2).GetPopulation(PopulationType::PREDATOR))
						{
							weights[0].first -= 1;
							weights[3].first -= 2;
							weights[2].first += 2;
						}

					//Down 1, Left 1
					if (currentCell.cellRow < height - 1 && currentCell.cellColumn >= 1)
						if (at(cellNum + width - 1).GetPopulation(PopulationType::PREDATOR))
						{
							weights[0].first -= 1;
							weights[2].first -= 2;
							weights[4].first -= 2;
							weights[1].first += 2;
							weights[3].first += 2;
						}

					//Down 1
					if (currentCell.cellRow < height - 1)
						if (at(cellNum + width).GetPopulation(PopulationType::PREDATOR))
						{
							weights[0].first -= 1;
							weights[4].first -= 2;
							weights[1].first += 2;
						}

					//Down 1, Right 1
					if (currentCell.cellRow < height - 1 && currentCell.cellColumn < width - 1)
						if (at(cellNum + width + 1).GetPopulation(PopulationType::PREDATOR))
						{
							weights[0].first -= 1;
							weights[3].first -= 2;
							weights[4].first -= 2;
							weights[1].first += 2;
							weights[2].first += 2;
						}

					//Down 2
					if (currentCell.cellRow < height - 2)
						if (at(cellNum + width * 2).GetPopulation(PopulationType::PREDATOR))
						{
							weights[0].first -= 1;
							weights[4].first -= 2;
							weights[1].first += 2;
						}
				}

				prio = currentCell.GetAnimal().GetFullness();

				//Childbearers
				if(currentCell.GetAnimal().wantsChild)
				{
					//Up 2
					if (currentCell.cellRow >= 2)
						if (at(cellNum - width * 2).GetPopulation(PopulationType::PREY) && at(cellNum - width * 2).animalWantsChild)
							weights[1].first += 1 * prio;

					//Up 1, Left 1
					if (currentCell.cellRow >= 1 && currentCell.cellColumn >= 1)
						if (at(cellNum - width - 1).GetPopulation(PopulationType::PREY) && at(cellNum - width - 1).animalWantsChild)
						{
							weights[1].first += 1 * prio;
							weights[2].first += 1 * prio;
						}

					//Up 1
					if (currentCell.cellRow >= 1)
						if (at(cellNum - width).GetPopulation(PopulationType::PREY) && at(cellNum - width).animalWantsChild)
							weights[0].first += 2 * prio;

					//Up 1, Right 1
					if (currentCell.cellRow >= 1 && currentCell.cellColumn < width - 1)
						if (at(cellNum - width + 1).GetPopulation(PopulationType::PREY) && at(cellNum - width + 1).animalWantsChild)
						{
							weights[1].first += 1 * prio;
							weights[3].first += 1 * prio;
						}

					//Left 2
					if (currentCell.cellColumn >= 2)
						if (at(cellNum - 2).GetPopulation(PopulationType::PREY) && at(cellNum - 2).animalWantsChild)
							weights[2].first += 1 * prio;

					//Left 1
					if (currentCell.cellColumn >= 1)
						if (at(cellNum - 1).GetPopulation(PopulationType::PREY) && at(cellNum - 1).animalWantsChild)
							weights[0].first += 2 * prio;

					//Right 1
					if (currentCell.cellColumn < width - 1)
						if (at(cellNum + 1).GetPopulation(PopulationType::PREY) && at(cellNum + 1).animalWantsChild)
							weights[0].first += 2 * prio;

					//Right 2
					if (currentCell.cellColumn < width - 2)
						if (at(cellNum + 2).GetPopulation(PopulationType::PREY) && at(cellNum + 2).animalWantsChild)
							weights[3].first += 1 * prio;

					//Down 1, Left 1
					if (currentCell.cellRow < height - 1 && currentCell.cellColumn >= 1)
						if (at(cellNum + width - 1).GetPopulation(PopulationType::PREY) && at(cellNum + width - 1).animalWantsChild)
						{
							weights[2].first += 1 * prio;
							weights[4].first += 1 * prio;
						}

					//Down 1
					if (currentCell.cellRow < height - 1)
						if (at(cellNum + width).GetPopulation(PopulationType::PREY) && at(cellNum + width).animalWantsChild)
							weights[0].first += 2 * prio;

					//Down 1, Right 1
					if (currentCell.cellRow < height - 1 && currentCell.cellColumn < width - 1)
						if (at(cellNum + width + 1).GetPopulation(PopulationType::PREY) && at(cellNum + width + 1).animalWantsChild)
						{
							weights[3].first += 1 * prio;
							weights[4].first += 1 * prio;
						}

					//Down 2
					if (currentCell.cellRow < height - 2)
						if (at(cellNum + width * 2).GetPopulation(PopulationType::PREY) && at(cellNum + width * 2).animalWantsChild)
							weights[4].first += 1 * prio;
				}
			}

			//Predator weights
			if(currentCell.GetAnimal().type == PopulationType::PREDATOR)
			{
				//Priority
				float prio = currentCell.GetAnimal().GetNeededFood();

				//Prey presence
				{
					//Up 2
					if (currentCell.cellRow >= 2)
						if (at(cellNum - width * 2).GetPopulation(PopulationType::PREY))
						{
							weights[1].first += 2 * prio;
						}

					//Up 1, Left 1
					if (currentCell.cellRow >= 1 && currentCell.cellColumn >= 1)
						if (at(cellNum - width - 1).GetPopulation(PopulationType::PREY))
						{
							weights[1].first += 2 * prio;
							weights[2].first += 2 * prio;
						}

					//Up 1
					if (currentCell.cellRow >= 1)
						if (at(cellNum - width).GetPopulation(PopulationType::PREY))
						{
							weights[0].first += 1 * prio;
							weights[1].first += 2 * prio;
						}

					//Up 1, Right 1
					if (currentCell.cellRow >= 1 && currentCell.cellColumn < width - 1)
						if (at(cellNum - width + 1).GetPopulation(PopulationType::PREY))
						{
							weights[1].first += 2 * prio;
							weights[3].first += 2 * prio;
						}

					//Left 2
					if (currentCell.cellColumn >= 2)
						if (at(cellNum - 2).GetPopulation(PopulationType::PREY))
						{
							weights[2].first += 2 * prio;
						}

					//Left 1
					if (currentCell.cellColumn >= 1)
						if (at(cellNum - 1).GetPopulation(PopulationType::PREY))
						{
							weights[0].first += 1 * prio;
							weights[2].first += 2 * prio;
						}

					//Right 1
					if (currentCell.cellColumn < width - 1)
						if (at(cellNum + 1).GetPopulation(PopulationType::PREY))
						{
							weights[0].first += 1 * prio;
							weights[3].first += 2 * prio;
						}

					//Right 2
					if (currentCell.cellColumn < width - 2)
						if (at(cellNum + 2).GetPopulation(PopulationType::PREY))
						{
							weights[3].first += 2 * prio;
						}

					//Down 1, Left 1
					if (currentCell.cellRow < height - 1 && currentCell.cellColumn >= 1)
						if (at(cellNum + width - 1).GetPopulation(PopulationType::PREY))
						{
							weights[2].first += 2 * prio;
							weights[4].first += 2 * prio;
						}

					//Down 1
					if (currentCell.cellRow < height - 1)
						if (at(cellNum + width).GetPopulation(PopulationType::PREY))
						{
							weights[4].first += 2 * prio;
						}

					//Down 1, Right 1
					if (currentCell.cellRow < height - 1 && currentCell.cellColumn < width - 1)
						if (at(cellNum + width + 1).GetPopulation(PopulationType::PREY))
						{
							weights[3].first += 2 * prio;
							weights[4].first += 2 * prio;
						}

					//Down 2
					if (currentCell.cellRow < height - 2)
						if (at(cellNum + width * 2).GetPopulation(PopulationType::PREY))
						{
							weights[4].first += 2 * prio;
						}
				}

				prio = currentCell.GetAnimal().GetFullness();

				//Childbearers
				if (currentCell.animalWantsChild)
				{
					//Up 2
					if (currentCell.cellRow >= 2)
						if (at(cellNum - width * 2).GetPopulation(PopulationType::PREDATOR) && at(cellNum - width * 2).animalWantsChild)
							weights[1].first += 2 * prio;

					//Up 1, Left 1
					if (currentCell.cellRow >= 1 && currentCell.cellColumn >= 1)
						if (at(cellNum - width - 1).GetPopulation(PopulationType::PREDATOR) && at(cellNum - width - 1).animalWantsChild)
						{
							weights[1].first += 2 * prio;
							weights[2].first += 2 * prio;
						}

					//Up 1
					if (currentCell.cellRow >= 1)
						if (at(cellNum - width).GetPopulation(PopulationType::PREDATOR) && at(cellNum - width).animalWantsChild)
							weights[0].first += 4 * prio;

					//Up 1, Right 1
					if (currentCell.cellRow >= 1 && currentCell.cellColumn < width - 1)
						if (at(cellNum - width + 1).GetPopulation(PopulationType::PREDATOR) && at(cellNum - width + 1).animalWantsChild)
						{
							weights[1].first += 2 * prio;
							weights[3].first += 2 * prio;
						}

					//Left 2
					if (currentCell.cellColumn >= 2)
						if (at(cellNum - 2).GetPopulation(PopulationType::PREDATOR) && at(cellNum - 2).animalWantsChild)
							weights[2].first += 2 * prio;

					//Left 1
					if (currentCell.cellColumn >= 1)
						if (at(cellNum - 1).GetPopulation(PopulationType::PREDATOR) && at(cellNum - 1).animalWantsChild)
							weights[0].first += 4 * prio;

					//Right 1
					if (currentCell.cellColumn < width - 1)
						if (at(cellNum + 1).GetPopulation(PopulationType::PREDATOR) && at(cellNum + 1).animalWantsChild)
							weights[0].first += 4 * prio;

					//Right 2
					if (currentCell.cellColumn < width - 2)
						if (at(cellNum + 2).GetPopulation(PopulationType::PREDATOR) && at(cellNum + 2).animalWantsChild)
							weights[3].first += 2 * prio;

					//Down 1, Left 1
					if (currentCell.cellRow < height - 1 && currentCell.cellColumn >= 1)
						if (at(cellNum + width - 1).GetPopulation(PopulationType::PREDATOR) && at(cellNum + width - 1).animalWantsChild)
						{
							weights[2].first += 2 * prio;
							weights[4].first += 2 * prio;
						}

					//Down 1
					if (currentCell.cellRow < height - 1)
						if (at(cellNum + width).GetPopulation(PopulationType::PREDATOR) && at(cellNum + width).animalWantsChild)
							weights[0].first += 4 * prio;

					//Down 1, Right 1
					if (currentCell.cellRow < height - 1 && currentCell.cellColumn < width - 1)
						if (at(cellNum + width + 1).GetPopulation(PopulationType::PREDATOR) && at(cellNum + width + 1).animalWantsChild)
						{
							weights[3].first += 2 * prio;
							weights[4].first += 2 * prio;
						}

					//Down 2
					if (currentCell.cellRow < height - 2)
						if (at(cellNum + width * 2).GetPopulation(PopulationType::PREDATOR) && at(cellNum + width * 2).animalWantsChild)
							weights[4].first += 2 * prio;
				}
			}

			//Randomise the order for the instances where they're all bad
			std::shuffle(weights.begin(), weights.end(), std::random_device());

			//Sort in descending order of weight
			for (int i = 1; i < weights.size(); i++)
			{
				std::pair<float, int> key = weights[i];
				int j = i - 1;

				while (j >= 0)
				{
					if (key.first < weights[j].first)
					{
						weights[j + 1] = weights[j];
						j--;
					}
					else
						break;
				}

				weights[j + 1] = key;
			}

			//Actual movement
			bool moved = false;
			while (!moved && !weights.empty())
			{
				switch (weights.back().second)
				{
				case 0://No movement
					moved = true;
					break;

				case 1://Up 1
					if (currentCell.cellRow >= 1)
					{
						if (!newWorld[cellNum - width].hasAnimal && (at(cellNum - width).cellType == CellType::LAND || currentCell.cellType != CellType::LAND))
						{
							newWorld[cellNum - width].AddAnimal(currentCell.GetAnimal());
							newWorld[cellNum].animalMoved();
							moved = true;
						}
					}

					break;

				case 2://Left 1
					if (currentCell.cellColumn >= 1)
					{
						if (!newWorld[cellNum - 1].hasAnimal && (at(cellNum - 1).cellType == CellType::LAND || currentCell.cellType != CellType::LAND))
						{
							newWorld[cellNum - 1].AddAnimal(currentCell.GetAnimal());
							newWorld[cellNum].animalMoved();
							moved = true;
						}
					}

					break;

				case 3://Right 1
					if (currentCell.cellColumn < width - 1)
					{
						if (!newWorld[cellNum + 1].hasAnimal && (at(cellNum + 1).cellType == CellType::LAND || currentCell.cellType != CellType::LAND))
						{
							newWorld[cellNum + 1].AddAnimal(currentCell.GetAnimal());
							newWorld[cellNum].animalMoved();
							moved = true;
						}
					}

					break;

				case 4://Down 1
					if (currentCell.cellRow < height - 1)
					{
						if (!newWorld[cellNum + width].hasAnimal && (at(cellNum + width).cellType == CellType::LAND || currentCell.cellType != CellType::LAND))
						{
							newWorld[cellNum + width].AddAnimal(currentCell.GetAnimal());
							newWorld[cellNum].animalMoved();
							moved = true;
						}
					}

					break;
				}

				weights.pop_back();
			}
		}

		//Predator Eating
		if(currentCell.predatorWantsFood && currentCell.hasAnimal)
		{
			bool preyPresence[4]{ false };
			int preyNum = 0;

			if (cellNum >= width)
				if (at(cellNum - width).hasAnimal)
					if(at(cellNum - width).GetAnimal().type == PopulationType::PREY)
						preyPresence[0] = true;

			if (cellNum % width > 0)
				if (at(cellNum - 1).hasAnimal)
					if (at(cellNum - 1).GetAnimal().type == PopulationType::PREY)
						preyPresence[1] = true;

			if (cellNum % width < width - 1)
				if (at(cellNum + 1).hasAnimal)
					if (at(cellNum + 1).GetAnimal().type == PopulationType::PREY)
						preyPresence[2] = true;

			if (cellNum < width * (height - 1))
				if (at(cellNum + width).hasAnimal)
					if (at(cellNum + width).GetAnimal().type == PopulationType::PREY)
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
					newWorld[cellNum].feedPredator(at(cellNum - width).GetAnimal().GetMass() * 4);
					newWorld[cellNum - width].killPrey();
					break;

				case 1:
					newWorld[cellNum].feedPredator(at(cellNum - 1).GetAnimal().GetMass() * 4);
					newWorld[cellNum - 1].killPrey();
					break;

				case 2:
					newWorld[cellNum].feedPredator(at(cellNum + 1).GetAnimal().GetMass() * 4);
					newWorld[cellNum + 1].killPrey();
					break;

				case 3:
					newWorld[cellNum].feedPredator(at(cellNum + width).GetAnimal().GetMass() * 4);
					newWorld[cellNum + width].killPrey();
					break;
				}
			}
		}

		//Animal Reproduction
		if (currentCell.animalWantsChild && currentCell.hasAnimal)
		{
			PopulationType childType = currentCell.GetAnimal().type;

			bool childBearers[4]{ false };
			int bearerNum = 0;

			if (cellNum >= width)
				if (at(cellNum - width).animalWantsChild && at(cellNum - width).hasAnimal)
					if (at(cellNum - width).GetAnimal().type == childType)
						childBearers[0] = true;

			if (cellNum % width > 0)
				if (at(cellNum - 1).animalWantsChild && at(cellNum - 1).hasAnimal)
					if (at(cellNum - 1).GetAnimal().type == childType)
						childBearers[1] = true;

			if (cellNum % width < width - 1)
				if (at(cellNum + 1).animalWantsChild && at(cellNum + 1).hasAnimal)
					if (at(cellNum + 1).GetAnimal().type == childType)
						childBearers[2] = true;

			if (cellNum < width * (height - 1))
				if (at(cellNum + width).animalWantsChild && at(cellNum + width).hasAnimal)
					if (at(cellNum + width).GetAnimal().type == childType)
						childBearers[3] = true;

			for (auto cell : childBearers)
				if (cell)
					bearerNum++;

			if (bearerNum > 0)
			{
				int bearerDir = rand() % bearerNum + 1;

				int bearerIndex = -1;
				while (bearerDir > 0)
				{
					bearerIndex++;
					if (childBearers[bearerIndex])
					{
						bearerDir--;
					}
				}

				bool bearer = rand() % 2;

				switch (bearerIndex)
				{
				case 0://Up
					if (bearer)//Children spawn next to this cell
					{
						if (cellNum % width > 0)
							if (!newWorld[cellNum - 1].hasAnimal && at(cellNum - 1).cellType == CellType::LAND)
								newWorld[cellNum - 1].AddAnimal(Animal(childType));

						if (cellNum % width < width - 1)
							if (!newWorld[cellNum + 1].hasAnimal && at(cellNum + 1).cellType == CellType::LAND)
								newWorld[cellNum + 1].AddAnimal(Animal(childType));
					}
					else
					{
						if (cellNum % width > 0)
							if (!newWorld[cellNum - width - 1].hasAnimal && at(cellNum - width - 1).cellType == CellType::LAND)
								newWorld[cellNum - width - 1].AddAnimal(Animal(childType));

						if (cellNum % width < width - 1)
							if (!newWorld[cellNum - width + 1].hasAnimal && at(cellNum - width + 1).cellType == CellType::LAND)
								newWorld[cellNum - width + 1].AddAnimal(Animal(childType));
					}

					break;

				case 1://Left
					if (bearer)//Children spawn next to this cell
					{
						if (cellNum >= width)
							if (!newWorld[cellNum - width].hasAnimal && at(cellNum - width).cellType == CellType::LAND)
								newWorld[cellNum - width].AddAnimal(Animal(childType));

						if (cellNum + width < width * (height - 1))
							if (!newWorld[cellNum + width].hasAnimal && at(cellNum + width).cellType == CellType::LAND)
								newWorld[cellNum + width].AddAnimal(Animal(childType));
					}
					else
					{
						if (cellNum >= width)
							if (!newWorld[cellNum - 1 - width].hasAnimal && at(cellNum - 1 - width).cellType == CellType::LAND)
								newWorld[cellNum - 1 - width].AddAnimal(Animal(childType));

						if (cellNum + width < width * (height - 1))
							if (!newWorld[cellNum - 1 + width].hasAnimal && at(cellNum - 1 + width).cellType == CellType::LAND)
								newWorld[cellNum - 1 + width].AddAnimal(Animal(childType));
					}

					break;

				case 2://Right
					if (bearer)//Children spawn next to this cell
					{
						if (cellNum >= width)
							if (!newWorld[cellNum - width].hasAnimal && at(cellNum - width).cellType == CellType::LAND)
								newWorld[cellNum - width].AddAnimal(Animal(childType));

						if (cellNum + width < width * (height - 1))
							if (!newWorld[cellNum + width].hasAnimal && at(cellNum + width).cellType == CellType::LAND)
								newWorld[cellNum + width].AddAnimal(Animal(childType));
					}
					else
					{
						if (cellNum >= width)
							if (!newWorld[cellNum + 1 - width].hasAnimal && at(cellNum + 1 - width).cellType == CellType::LAND)
								newWorld[cellNum + 1 - width].AddAnimal(Animal(childType));

						if (cellNum + width < width * (height - 1))
							if (!newWorld[cellNum + 1 + width].hasAnimal && at(cellNum + 1 + width).cellType == CellType::LAND)
								newWorld[cellNum + 1 + width].AddAnimal(Animal(childType));
					}

					break;

				case 3://Down
					if (bearer)//Children spawn next to this cell
					{
						if (cellNum % width > 0)
							if (!newWorld[cellNum - 1].hasAnimal && at(cellNum - 1).cellType == CellType::LAND)
								newWorld[cellNum - 1].AddAnimal(Animal(childType));

						if (cellNum % width < width - 1)
							if (!newWorld[cellNum + 1].hasAnimal && at(cellNum + 1).cellType == CellType::LAND)
								newWorld[cellNum + 1].AddAnimal(Animal(childType));
					}
					else
					{
						if (cellNum % width > 0)
							if (!newWorld[cellNum + width - 1].hasAnimal && at(cellNum + width - 1).cellType == CellType::LAND)
								newWorld[cellNum + width - 1].AddAnimal(Animal(childType));

						if (cellNum % width < width - 1)
							if (!newWorld[cellNum + width + 1].hasAnimal && at(cellNum + width + 1).cellType == CellType::LAND)
								newWorld[cellNum + width + 1].AddAnimal(Animal(childType));
					}

					break;
				}

				newWorld[cellNum].animalHadChild();
			}
		}

		newWorld[cellNum].Update();
	}

	return newWorld;
}

int EcoResilience::World::GetPopulation(PopulationType type)
{
	int population = 0;

	for (int cellNum = 0; cellNum < size(); cellNum++)
	{
		population += at(cellNum).GetPopulation(type);
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
				float water = float((rand() % 1000)) / 10000.f;
				at(row * width + column).SetWater(water + at(row * width + column).GetWater());
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
				float water = (CPerlinNoise::noise2(waterVec) + 0.5f) * 0.1f;

				at(row * width + column).SetWater(water + at(row * width + column).GetWater());
			}
		}
	}
}

void EcoResilience::World::UrbanDevelop()
{
	for(auto cell : *this)
		if(std::pow(cell.cellColumn, 2) + std::pow(cell.cellRow, 2) <= std::pow(width / 2, 2))
			at(cell.cellRow * width + cell.cellColumn).cellType = CellType::URBANISED;
}
