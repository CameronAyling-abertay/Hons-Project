#include "World.h"
#include <random>
#include <vector>
#include "CPerlinNoise/CPerlinNoise.h"

EcoResilience::World::World()
{}

void EcoResilience::World::Generate(EcoSize size, EcoParameters parameters)
{
	//Clear all data - function from vector
	clear();

	worldSize = size;
	worldParameters = parameters;

	//Generate the world
	//Random generation will randomly allocate cells water levels
	//Perlin generation will generate it using perlin noise for a more natural look
	switch (worldParameters.genType)
	{
	case GenerationType::RANDOM://Random generation
		for (int row = 0; row < worldSize.height; row++)
		{
			for (int column = 0; column < worldSize.width; column++)
			{
				Cell newCell(row, column, worldSize.maxMass);

				//Add water
				float water = float((rand() % 1000)) / 1000.f;
				newCell.SetWater(water);

				//Set Altitude
				float altitude = float(rand() % 1000 / 1000.f) * 0.55 + 0.45;
				newCell.SetAltitude(altitude);

				//Create plants
				float plantMass = ((rand() % 1000) / 1000.f) * worldSize.maxMass;

				int iterations = 0;
				while(iterations < 100)
				{
					Plant newPlant;
					newPlant.TopMass(worldSize.maxMass);

					if (newPlant.GetMass() > std::max(plantMass - worldSize.maxMass * 0.05f, 0.f) && newPlant.GetMass() < std::min(plantMass + worldSize.maxMass * 0.05f, worldSize.maxMass))
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

	case GenerationType::PERLIN://Perlin generation
		const float waterOffset = rand();
		const float altOffset = rand();
		const float plantOffset = rand();
		const float preyOffset = rand();
		const float predatorOffset = rand();

		for (int row = 0; row < worldSize.height; row++)
		{
			for (int column = 0; column < worldSize.width; column++)
			{
				Cell newCell(row, column, worldSize.maxMass);

				//Create the water level
				float waterVec[2]{ (row + waterOffset) * worldParameters.waterMod, (column + waterOffset) * worldParameters.waterMod };
				float water = CPerlinNoise::noise2(waterVec) + 0.5f;

				newCell.SetWater(water);

				//Set altitude
				float altVec[2]{ (row + altOffset) * worldParameters.altitudeMod, (column + altOffset) * worldParameters.altitudeMod };
				float altitude = (CPerlinNoise::noise2(altVec) + 0.5f) * 0.55 + 0.45;

				newCell.SetAltitude(altitude);

				//Create the plants
				float plantVec[2]{ (row + plantOffset) * worldParameters.plantMod, (column + plantOffset) * worldParameters.plantMod };
				float plantMass = (CPerlinNoise::noise2(plantVec) + 0.5f) * worldSize.maxMass;

				int iterations = 0;
				while (iterations < 100)
				{
					Plant newPlant;
					newPlant.TopMass(worldSize.maxMass);

					if (newPlant.GetMass() > std::max(plantMass - worldSize.maxMass * 0.05f, 0.f) && newPlant.GetMass() < std::min(plantMass + worldSize.maxMass * 0.05f, worldSize.maxMass))
					{
						newCell.AddPlant(newPlant);
						break;
					}

					iterations++;
				}

				//Create the animals
				float preyVec[2]{ (row + preyOffset) * parameters.preyMod, (column + preyOffset) * parameters.preyMod };
				int preyPop = (CPerlinNoise::noise2(preyVec) + 0.5f) * 10;

				if(preyPop <= 1 || preyPop >= 9)
					newCell.AddAnimal(Animal(PopulationType::PREY));

				float predatorVec[2]{ (row + predatorOffset) * parameters.predatorMod, (column + predatorOffset) * parameters.predatorMod };
				int predatorPop = (CPerlinNoise::noise2(predatorVec) + 0.5f) * 10;

				if (predatorPop == 0 || predatorPop == 9)
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
	//Create the back buffer
	World newWorld = *this;

	for (int cellNum = 0; cellNum < worldSize.width * worldSize.height; cellNum++)
	{
		//Cut down on world object calls by using a reference to the cell in question
		Cell currentCell = at(cellNum);

		//Fire Spread
		if (currentCell.hasPlant)
		{
			if (currentCell.GetPlant().fire)
			{
				//Up
				if (rand() % 100 < 80)
					if (cellNum >= worldSize.width)
						if (at(cellNum - worldSize.width).hasPlant && at(cellNum - worldSize.width).cellType == CellType::LAND)
							newWorld[cellNum - worldSize.width].SetFire();

				//Left
				if (rand() % 100 < 80)
					if (cellNum % worldSize.width > 0)
						if (at(cellNum - 1).hasPlant && at(cellNum - 1).cellType == CellType::LAND)
							newWorld[cellNum - 1].SetFire();

				//Right
				if (rand() % 100 < 80)
					if (cellNum % worldSize.width < worldSize.width - 1)
						if (at(cellNum + 1).hasPlant && at(cellNum + 1).cellType == CellType::LAND)
							newWorld[cellNum + 1].SetFire();

				//Down
				if (rand() % 100 < 80)
					if (cellNum < worldSize.width * (worldSize.height - 1))
						if (at(cellNum + worldSize.width).hasPlant && at(cellNum + worldSize.width).cellType == CellType::LAND)
							newWorld[cellNum + worldSize.width].SetFire();
			}
		}

		//Plague Spread
		if (currentCell.hasAnimal)
		{
			if (currentCell.GetAnimal().infected)
			{
				//Up
				if (rand() % 100 < 80)
					if (cellNum >= worldSize.width)
						if (at(cellNum - worldSize.width).hasAnimal)
							newWorld[cellNum - worldSize.width].InfectAnimal();

				//Left
				if (rand() % 100 < 80)
					if (cellNum % worldSize.width > 0)
						if (at(cellNum - 1).hasAnimal)
							newWorld[cellNum - 1].InfectAnimal();

				//Right
				if (rand() % 100 < 80)
					if (cellNum % worldSize.width < worldSize.width - 1)
						if (at(cellNum + 1).hasAnimal)
							newWorld[cellNum + 1].InfectAnimal();

				//Down
				if (rand() % 100 < 80)
					if (cellNum < worldSize.width * (worldSize.height - 1))
						if (at(cellNum + worldSize.width).hasAnimal)
							newWorld[cellNum + worldSize.width].InfectAnimal();
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
					if (cellNum >= worldSize.width * 2)
					{
						if (!at(cellNum - worldSize.width * 2).hasPlant)
						{
							Plant newPlant(currentCell.GetPlant().GetVigor() * 0.1);
							newWorld[cellNum - worldSize.width * 2].AddPlant(newPlant);
							newWorld[cellNum].plantHadChild();
							hadChild = true;
						}
					}
					break;

				case 2:// Up 1, Left 1
					if (cellNum >= worldSize.width && cellNum % worldSize.width > 0)
					{
						if (!at(cellNum - worldSize.width - 1).hasPlant)
						{
							Plant newPlant(currentCell.GetPlant().GetVigor() * 0.1);
							newWorld[cellNum - worldSize.width - 1].AddPlant(newPlant);
							newWorld[cellNum].plantHadChild();
							hadChild = true;
						}
					}
					break;

				case 3:// Up 1
					if (cellNum >= worldSize.width)
					{
						if (!at(cellNum - worldSize.width).hasPlant)
						{
							Plant newPlant(currentCell.GetPlant().GetVigor() * 0.1);
							newWorld[cellNum - worldSize.width].AddPlant(newPlant);
							newWorld[cellNum].plantHadChild();
							hadChild = true;
						}
					}
					break;

				case 4:// Up 1, Right 1
					if (cellNum >= worldSize.width && cellNum % worldSize.width < worldSize.width - 1)
					{
						if (!at(cellNum - worldSize.width + 1).hasPlant)
						{
							Plant newPlant(currentCell.GetPlant().GetVigor() * 0.1);
							newWorld[cellNum - worldSize.width + 1].AddPlant(newPlant);
							newWorld[cellNum].plantHadChild();
							hadChild = true;
						}
					}
					break;

				case 5:// Left 2
					if (cellNum % worldSize.width > 1)
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
					if (cellNum % worldSize.width > 0)
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
					if (cellNum % worldSize.width < worldSize.width - 1)
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
					if (cellNum % worldSize.width < worldSize.width - 2)
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
					if (cellNum < worldSize.width * (worldSize.height - 1) && cellNum % worldSize.width > 0)
					{
						if (!at(cellNum + worldSize.width - 1).hasPlant)
						{
							Plant newPlant(currentCell.GetPlant().GetVigor() * 0.1);
							newWorld[cellNum + worldSize.width - 1].AddPlant(newPlant);
							newWorld[cellNum].plantHadChild();
							hadChild = true;
						}
					}
					break;

				case 10:// Down 1
					if (cellNum < worldSize.width * (worldSize.height - 1))
					{
						if (!at(cellNum + worldSize.width).hasPlant)
						{
							Plant newPlant(currentCell.GetPlant().GetVigor() * 0.1);
							newWorld[cellNum + worldSize.width].AddPlant(newPlant);
							newWorld[cellNum].plantHadChild();
							hadChild = true;
						}
					}
					break;

				case 11:// Down 1, Right 1
					if (cellNum < worldSize.width * (worldSize.height - 1) && cellNum % worldSize.width < worldSize.width - 1)
					{
						if (!at(cellNum + worldSize.width + 1).hasPlant)
						{
							Plant newPlant(currentCell.GetPlant().GetVigor() * 0.1);
							newWorld[cellNum + worldSize.width + 1].AddPlant(newPlant);
							newWorld[cellNum].plantHadChild();
							hadChild = true;
						}
					}
					break;

				case 12:// Down 2
					if (cellNum + worldSize.width < worldSize.width * (worldSize.height - 1))
					{
						if (!at(cellNum + worldSize.width * 2).hasPlant)
						{
							Plant newPlant(currentCell.GetPlant().GetVigor() * 0.1);
							newWorld[cellNum + worldSize.width * 2].AddPlant(newPlant);
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
				//Plant mass
				{
					float prio = currentCell.GetAnimal().GetNeededFood();

					//Up 2
					if (currentCell.cellRow >= 2)
						if (at((currentCell.cellRow - 2) * worldSize.width + currentCell.cellColumn).hasPlant)
							weights.at(1).first += at((currentCell.cellRow - 2) * worldSize.width + currentCell.cellColumn).GetPlant().GetMass() * prio;

					//Up 1, Left 1
					if (currentCell.cellRow >= 1 && currentCell.cellColumn >= 1)
						if (at((currentCell.cellRow - 1) * worldSize.width + (currentCell.cellColumn - 1)).hasPlant)
						{
							weights.at(1).first += at((currentCell.cellRow - 1) * worldSize.width + (currentCell.cellColumn - 1)).GetPlant().GetMass() * prio;
							weights.at(2).first += at((currentCell.cellRow - 1) * worldSize.width + (currentCell.cellColumn - 1)).GetPlant().GetMass() * prio;
						}

					//Up 1
					if (currentCell.cellRow >= 1)
						if (at((currentCell.cellRow - 1) * worldSize.width + currentCell.cellColumn).hasPlant)
							weights.at(1).first += at((currentCell.cellRow - 1) * worldSize.width + currentCell.cellColumn).GetPlant().GetMass() * prio;

					//Up 1, Right 1
					if (currentCell.cellRow >= 1 && currentCell.cellColumn < worldSize.width - 1)
						if (at((currentCell.cellRow - 1) * worldSize.width + (currentCell.cellColumn + 1)).hasPlant)
						{
							weights.at(1).first += at((currentCell.cellRow - 1) * worldSize.width + (currentCell.cellColumn + 1)).GetPlant().GetMass() * prio;
							weights.at(3).first += at((currentCell.cellRow - 1) * worldSize.width + (currentCell.cellColumn + 1)).GetPlant().GetMass() * prio;
						}

					//Left 2
					if (currentCell.cellColumn >= 2)
						if (at(currentCell.cellRow * worldSize.width + (currentCell.cellColumn - 2)).hasPlant)
							weights.at(2).first += at(currentCell.cellRow * worldSize.width + (currentCell.cellColumn - 2)).GetPlant().GetMass() * prio;

					//Left 1
					if (currentCell.cellColumn >= 1)
						if (at(currentCell.cellRow * worldSize.width + (currentCell.cellColumn - 1)).hasPlant)
							weights.at(2).first += at(currentCell.cellRow * worldSize.width + (currentCell.cellColumn - 1)).GetPlant().GetMass() * prio;

					//This cell
					if (currentCell.hasPlant)
						weights.at(0).first += currentCell.GetPlant().GetMass() * prio;

					//Right 1
					if (currentCell.cellColumn < worldSize.width - 1)
						if (at(currentCell.cellRow * worldSize.width + (currentCell.cellColumn + 1)).hasPlant)
							weights.at(3).first += at(currentCell.cellRow * worldSize.width + (currentCell.cellColumn + 1)).GetPlant().GetMass() * prio;

					//Right 2
					if (currentCell.cellColumn < worldSize.width - 2)
						if (at(currentCell.cellRow * worldSize.width + (currentCell.cellColumn + 2)).hasPlant)
							weights.at(3).first += at(currentCell.cellRow * worldSize.width + (currentCell.cellColumn + 2)).GetPlant().GetMass() * prio;

					//Down 1, Left 1
					if (currentCell.cellRow < worldSize.height - 1 && currentCell.cellColumn >= 1)
						if (at((currentCell.cellRow + 1) * worldSize.width + (currentCell.cellColumn - 1)).hasPlant)
						{
							weights.at(2).first += at((currentCell.cellRow + 1) * worldSize.width + (currentCell.cellColumn - 1)).GetPlant().GetMass() * prio;
							weights.at(4).first += at((currentCell.cellRow + 1) * worldSize.width + (currentCell.cellColumn - 1)).GetPlant().GetMass() * prio;
						}

					//Down 1
					if (currentCell.cellRow < worldSize.height - 1)
						if (at((currentCell.cellRow + 1) * worldSize.width + currentCell.cellColumn).hasPlant)
							weights.at(4).first += at((currentCell.cellRow + 1) * worldSize.width).GetPlant().GetMass() * prio;

					//Down 1, Right 1
					if (currentCell.cellRow < worldSize.height - 1 && currentCell.cellColumn < worldSize.width - 1)
						if (at((currentCell.cellRow + 1) * worldSize.width + (currentCell.cellColumn + 1)).hasPlant)
						{
							weights.at(3).first += at((currentCell.cellRow + 1) * worldSize.width + (currentCell.cellColumn + 1)).GetPlant().GetMass() * prio;
							weights.at(4).first += at((currentCell.cellRow + 1) * worldSize.width + (currentCell.cellColumn + 1)).GetPlant().GetMass() * prio;
						}

					//Down 2
					if (currentCell.cellRow < worldSize.height - 2)
						if (at((currentCell.cellRow + 2) * worldSize.width + currentCell.cellColumn).hasPlant)
							weights.at(4).first += at((currentCell.cellRow + 2) * worldSize.width).GetPlant().GetMass() * prio;
				}

				//Predator presence
				{
					PopulationType predatorType = PopulationType::PREDATOR;

					//Up 2
					if (currentCell.cellRow >= 2)
						if (at((currentCell.cellRow - 2) * worldSize.width + currentCell.cellColumn).GetPopulation(predatorType))
						{
							weights.at(0).first -= 1;
							weights.at(1).first -= 2;
							weights.at(4).first += 2;
						}

					//Up 1, Left 1
					if (currentCell.cellRow >= 1 && currentCell.cellColumn >= 1)
						if (at((currentCell.cellRow - 1) * worldSize.width + (currentCell.cellColumn - 1)).GetPopulation(predatorType))
						{
							weights.at(0).first -= 1;
							weights.at(1).first -= 2;
							weights.at(2).first -= 2;
							weights.at(3).first += 2;
							weights.at(4).first += 2;
						}

					//Up 1
					if (currentCell.cellRow >= 1)
						if (at((currentCell.cellRow - 1) * worldSize.width + currentCell.cellColumn).GetPopulation(predatorType))
						{
							weights.at(0).first -= 1;
							weights.at(1).first -= 2;
							weights.at(4).first += 2;
						}

					//Up 1, Right 1
					if (currentCell.cellRow >= 1 && currentCell.cellColumn < worldSize.width - 1)
						if (at((currentCell.cellRow - 1) * worldSize.width + (currentCell.cellColumn + 1)).GetPopulation(predatorType))
						{
							weights.at(0).first -= 1;
							weights.at(1).first -= 2;
							weights.at(3).first -= 2;
							weights.at(2).first += 2;
							weights.at(4).first += 2;
						}

					//Left 2
					if (currentCell.cellColumn >= 2)
						if (at(currentCell.cellRow * worldSize.width + (currentCell.cellColumn - 2)).GetPopulation(predatorType))
						{
							weights.at(0).first -= 1;
							weights.at(2).first -= 2;
							weights.at(3).first += 2;
						}

					//Left 1
					if (currentCell.cellColumn >= 1)
						if (at(currentCell.cellRow * worldSize.width + (currentCell.cellColumn - 1)).GetPopulation(predatorType))
						{
							weights.at(0).first -= 1;
							weights.at(2).first -= 2;
							weights.at(3).first += 2;
						}

					//Right 1
					if (currentCell.cellColumn < worldSize.width - 1)
						if (at(currentCell.cellRow * worldSize.width + (currentCell.cellColumn + 1)).GetPopulation(predatorType))
						{
							weights.at(0).first -= 1;
							weights.at(3).first -= 2;
							weights.at(2).first += 2;
						}

					//Right 2
					if (currentCell.cellColumn < worldSize.width - 2)
						if (at(currentCell.cellRow * worldSize.width + (currentCell.cellColumn + 2)).GetPopulation(predatorType))
						{
							weights.at(0).first -= 1;
							weights.at(3).first -= 2;
							weights.at(2).first += 2;
						}

					//Down 1, Left 1
					if (currentCell.cellRow < worldSize.height - 1 && currentCell.cellColumn >= 1)
						if (at((currentCell.cellRow + 1) * worldSize.width + (currentCell.cellColumn - 1)).GetPopulation(predatorType))
						{
							weights.at(0).first -= 1;
							weights.at(2).first -= 2;
							weights.at(4).first -= 2;
							weights.at(1).first += 2;
							weights.at(3).first += 2;
						}

					//Down 1
					if (currentCell.cellRow < worldSize.height - 1)
						if (at((currentCell.cellRow + 1) * worldSize.width + currentCell.cellColumn).GetPopulation(predatorType))
						{
							weights.at(0).first -= 1;
							weights.at(4).first -= 2;
							weights.at(1).first += 2;
						}

					//Down 1, Right 1
					if (currentCell.cellRow < worldSize.height - 1 && currentCell.cellColumn < worldSize.width - 1)
						if (at((currentCell.cellRow + 1) * worldSize.width + (currentCell.cellColumn + 1)).GetPopulation(predatorType))
						{
							weights.at(0).first -= 1;
							weights.at(3).first -= 2;
							weights.at(4).first -= 2;
							weights.at(1).first += 2;
							weights.at(2).first += 2;
						}

					//Down 2
					if (currentCell.cellRow < worldSize.height - 2)
						if (at((currentCell.cellRow + 2) * worldSize.width + currentCell.cellColumn).GetPopulation(predatorType))
						{
							weights.at(0).first -= 1;
							weights.at(4).first -= 2;
							weights.at(1).first += 2;
						}
				}

				//Childbearers
				if (currentCell.GetAnimal().wantsChild)

				{
					float prio = currentCell.GetAnimal().GetFullness();
					PopulationType animalType = currentCell.GetAnimal().type;

					//Up 2
					if (currentCell.cellRow >= 2)
						if (at((currentCell.cellRow - 2) * worldSize.width + currentCell.cellColumn).GetPopulation(animalType) && at((currentCell.cellRow - 2) * worldSize.width + currentCell.cellColumn).animalWantsChild)
							weights.at(1).first += 1 * prio;

					//Up 1, Left 1
					if (currentCell.cellRow >= 1 && currentCell.cellColumn >= 1)
						if (at((currentCell.cellRow - 1) * worldSize.width + (currentCell.cellColumn - 1)).GetPopulation(animalType) && at((currentCell.cellRow - 1) * worldSize.width + (currentCell.cellColumn - 1)).animalWantsChild)
						{
							weights.at(1).first += 1 * prio;
							weights.at(2).first += 1 * prio;
						}

					//Up 1
					if (currentCell.cellRow >= 1)
						if (at((currentCell.cellRow - 1) * worldSize.width + currentCell.cellColumn).GetPopulation(animalType) && at((currentCell.cellRow - 1) * worldSize.width + currentCell.cellColumn).animalWantsChild)
							weights.at(0).first += 2 * prio;

					//Up 1, Right 1
					if (currentCell.cellRow >= 1 && currentCell.cellColumn < worldSize.width - 1)
						if (at((currentCell.cellRow - 1) * worldSize.width + (currentCell.cellColumn + 1)).GetPopulation(animalType) && at((currentCell.cellRow - 1) * worldSize.width + (currentCell.cellColumn + 1)).animalWantsChild)
						{
							weights.at(1).first += 1 * prio;
							weights.at(3).first += 1 * prio;
						}

					//Left 2
					if (currentCell.cellColumn >= 2)
						if (at(currentCell.cellRow * worldSize.width + (currentCell.cellColumn - 2)).GetPopulation(animalType) && at(currentCell.cellRow * worldSize.width + (currentCell.cellColumn - 2)).animalWantsChild)
							weights.at(2).first += 1 * prio;

					//Left 1
					if (currentCell.cellColumn >= 1)
						if (at(currentCell.cellRow * worldSize.width + (currentCell.cellColumn - 1)).GetPopulation(animalType) && at(currentCell.cellRow * worldSize.width + (currentCell.cellColumn - 1)).animalWantsChild)
							weights.at(0).first += 2 * prio;

					//Right 1
					if (currentCell.cellColumn < worldSize.width - 1)
						if (at(currentCell.cellRow * worldSize.width + (currentCell.cellColumn + 1)).GetPopulation(animalType) && at(currentCell.cellRow * worldSize.width + (currentCell.cellColumn + 1)).animalWantsChild)
							weights.at(0).first += 2 * prio;

					//Right 2
					if (currentCell.cellColumn < worldSize.width - 2)
						if (at(currentCell.cellRow * worldSize.width + (currentCell.cellColumn + 2)).GetPopulation(animalType) && at(currentCell.cellRow * worldSize.width + (currentCell.cellColumn + 2)).animalWantsChild)
							weights.at(3).first += 1 * prio;

					//Down 1, Left 1
					if (currentCell.cellRow < worldSize.height - 1 && currentCell.cellColumn >= 1)
						if (at((currentCell.cellRow + 1) * worldSize.width + (currentCell.cellColumn - 1)).GetPopulation(animalType) && at((currentCell.cellRow + 1) * worldSize.width + (currentCell.cellColumn - 1)).animalWantsChild)
						{
							weights.at(2).first += 1 * prio;
							weights.at(4).first += 1 * prio;
						}

					//Down 1
					if (currentCell.cellRow < worldSize.height - 1)
						if (at((currentCell.cellRow + 1) * worldSize.width + currentCell.cellColumn).GetPopulation(animalType) && at((currentCell.cellRow + 1) * worldSize.width + currentCell.cellColumn).animalWantsChild)
							weights.at(0).first += 2 * prio;

					//Down 1, Right 1
					if (currentCell.cellRow < worldSize.height - 1 && currentCell.cellColumn < worldSize.width - 1)
						if (at((currentCell.cellRow + 1) * worldSize.width + (currentCell.cellColumn + 1)).GetPopulation(animalType) && at((currentCell.cellRow + 1) * worldSize.width + (currentCell.cellColumn + 1)).animalWantsChild)
						{
							weights.at(3).first += 1 * prio;
							weights.at(4).first += 1 * prio;
						}

					//Down 2
					if (currentCell.cellRow < worldSize.height - 2)
						if (at((currentCell.cellRow + 2) * worldSize.width + currentCell.cellColumn).GetPopulation(animalType) && at((currentCell.cellRow + 2) * worldSize.width + currentCell.cellColumn).animalWantsChild)
							weights.at(4).first += 1 * prio;
				}
			}

			//Predator weights
			if(currentCell.GetAnimal().type == PopulationType::PREDATOR)
			{
				//Prey presence
				{
					float prio = currentCell.GetAnimal().GetNeededFood();
					PopulationType preyType = PopulationType::PREY;

					//Up 2
					if (currentCell.cellRow >= 2)
						if (at((currentCell.cellRow - 2) * worldSize.width + currentCell.cellColumn).GetPopulation(preyType))
						{
							weights.at(1).first += 2 * prio;
						}

					//Up 1, Left 1
					if (currentCell.cellRow >= 1 && currentCell.cellColumn >= 1)
						if (at((currentCell.cellRow - 1) * worldSize.width + (currentCell.cellColumn - 1)).GetPopulation(preyType))
						{
							weights.at(1).first += 2 * prio;
							weights.at(2).first += 2 * prio;
						}

					//Up 1
					if (currentCell.cellRow >= 1)
						if (at((currentCell.cellRow - 1) * worldSize.width + currentCell.cellColumn).GetPopulation(preyType))
						{
							weights.at(0).first += 1 * prio;
							weights.at(1).first += 2 * prio;
						}

					//Up 1, Right 1
					if (currentCell.cellRow >= 1 && currentCell.cellColumn < worldSize.width - 1)
						if (at((currentCell.cellRow - 1) * worldSize.width + (currentCell.cellColumn + 1)).GetPopulation(preyType))
						{
							weights.at(1).first += 2 * prio;
							weights.at(3).first += 2 * prio;
						}

					//Left 2
					if (currentCell.cellColumn >= 2)
						if (at(currentCell.cellRow * worldSize.width + (currentCell.cellColumn - 2)).GetPopulation(preyType))
						{
							weights.at(2).first += 2 * prio;
						}

					//Left 1
					if (currentCell.cellColumn >= 1)
						if (at(currentCell.cellRow * worldSize.width + (currentCell.cellColumn - 1)).GetPopulation(preyType))
						{
							weights.at(0).first += 1 * prio;
							weights.at(2).first += 2 * prio;
						}

					//Right 1
					if (currentCell.cellColumn < worldSize.width - 1)
						if (at(currentCell.cellRow * worldSize.width + (currentCell.cellColumn + 1)).GetPopulation(preyType))
						{
							weights.at(0).first += 1 * prio;
							weights.at(3).first += 2 * prio;
						}

					//Right 2
					if (currentCell.cellColumn < worldSize.width - 2)
						if (at(currentCell.cellRow * worldSize.width + (currentCell.cellColumn + 2)).GetPopulation(preyType))
						{
							weights.at(3).first += 2 * prio;
						}

					//Down 1, Left 1
					if (currentCell.cellRow < worldSize.height - 1 && currentCell.cellColumn >= 1)
						if (at((currentCell.cellRow + 1) * worldSize.width + (currentCell.cellColumn - 1)).GetPopulation(preyType))
						{
							weights.at(2).first += 2 * prio;
							weights.at(4).first += 2 * prio;
						}

					//Down 1
					if (currentCell.cellRow < worldSize.height - 1)
						if (at((currentCell.cellRow + 1) * worldSize.width + currentCell.cellColumn).GetPopulation(preyType))
						{
							weights.at(4).first += 2 * prio;
						}

					//Down 1, Right 1
					if (currentCell.cellRow < worldSize.height - 1 && currentCell.cellColumn < worldSize.width - 1)
						if (at((currentCell.cellRow + 1) * worldSize.width + (currentCell.cellColumn + 1)).GetPopulation(preyType))
						{
							weights.at(3).first += 2 * prio;
							weights.at(4).first += 2 * prio;
						}

					//Down 2
					if (currentCell.cellRow < worldSize.height - 2)
						if (at((currentCell.cellRow + 2) * worldSize.width + currentCell.cellColumn).GetPopulation(preyType))
						{
							weights.at(4).first += 2 * prio;
						}
				}

				//Childbearers
				if (currentCell.GetAnimal().wantsChild)

				{
					float prio = currentCell.GetAnimal().GetFullness() * 2;
					PopulationType animalType = currentCell.GetAnimal().type;

					//Up 2
					if (currentCell.cellRow >= 2)
						if (at((currentCell.cellRow - 2) * worldSize.width + currentCell.cellColumn).GetPopulation(animalType) && at((currentCell.cellRow - 2) * worldSize.width + currentCell.cellColumn).animalWantsChild)
							weights.at(1).first += 1 * prio;

					//Up 1, Left 1
					if (currentCell.cellRow >= 1 && currentCell.cellColumn >= 1)
						if (at((currentCell.cellRow - 1) * worldSize.width + (currentCell.cellColumn - 1)).GetPopulation(animalType) && at((currentCell.cellRow - 1) * worldSize.width + (currentCell.cellColumn - 1)).animalWantsChild)
						{
							weights.at(1).first += 1 * prio;
							weights.at(2).first += 1 * prio;
						}

					//Up 1
					if (currentCell.cellRow >= 1)
						if (at((currentCell.cellRow - 1) * worldSize.width + currentCell.cellColumn).GetPopulation(animalType) && at((currentCell.cellRow - 1) * worldSize.width + currentCell.cellColumn).animalWantsChild)
							weights.at(0).first += 2 * prio;

					//Up 1, Right 1
					if (currentCell.cellRow >= 1 && currentCell.cellColumn < worldSize.width - 1)
						if (at((currentCell.cellRow - 1) * worldSize.width + (currentCell.cellColumn + 1)).GetPopulation(animalType) && at((currentCell.cellRow - 1) * worldSize.width + (currentCell.cellColumn + 1)).animalWantsChild)
						{
							weights.at(1).first += 1 * prio;
							weights.at(3).first += 1 * prio;
						}

					//Left 2
					if (currentCell.cellColumn >= 2)
						if (at(currentCell.cellRow * worldSize.width + (currentCell.cellColumn - 2)).GetPopulation(animalType) && at(currentCell.cellRow * worldSize.width + (currentCell.cellColumn - 2)).animalWantsChild)
							weights.at(2).first += 1 * prio;

					//Left 1
					if (currentCell.cellColumn >= 1)
						if (at(currentCell.cellRow * worldSize.width + (currentCell.cellColumn - 1)).GetPopulation(animalType) && at(currentCell.cellRow * worldSize.width + (currentCell.cellColumn - 1)).animalWantsChild)
							weights.at(0).first += 2 * prio;

					//Right 1
					if (currentCell.cellColumn < worldSize.width - 1)
						if (at(currentCell.cellRow * worldSize.width + (currentCell.cellColumn + 1)).GetPopulation(animalType) && at(currentCell.cellRow * worldSize.width + (currentCell.cellColumn + 1)).animalWantsChild)
							weights.at(0).first += 2 * prio;

					//Right 2
					if (currentCell.cellColumn < worldSize.width - 2)
						if (at(currentCell.cellRow * worldSize.width + (currentCell.cellColumn + 2)).GetPopulation(animalType) && at(currentCell.cellRow * worldSize.width + (currentCell.cellColumn + 2)).animalWantsChild)
							weights.at(3).first += 1 * prio;

					//Down 1, Left 1
					if (currentCell.cellRow < worldSize.height - 1 && currentCell.cellColumn >= 1)
						if (at((currentCell.cellRow + 1) * worldSize.width + (currentCell.cellColumn - 1)).GetPopulation(animalType) && at((currentCell.cellRow + 1) * worldSize.width + (currentCell.cellColumn - 1)).animalWantsChild)
						{
							weights.at(2).first += 1 * prio;
							weights.at(4).first += 1 * prio;
						}

					//Down 1
					if (currentCell.cellRow < worldSize.height - 1)
						if (at((currentCell.cellRow + 1) * worldSize.width + currentCell.cellColumn).GetPopulation(animalType) && at((currentCell.cellRow + 1) * worldSize.width + currentCell.cellColumn).animalWantsChild)
							weights.at(0).first += 2 * prio;

					//Down 1, Right 1
					if (currentCell.cellRow < worldSize.height - 1 && currentCell.cellColumn < worldSize.width - 1)
						if (at((currentCell.cellRow + 1) * worldSize.width + (currentCell.cellColumn + 1)).GetPopulation(animalType) && at((currentCell.cellRow + 1) * worldSize.width + (currentCell.cellColumn + 1)).animalWantsChild)
						{
							weights.at(3).first += 1 * prio;
							weights.at(4).first += 1 * prio;
						}

					//Down 2
					if (currentCell.cellRow < worldSize.height - 2)
						if (at((currentCell.cellRow + 2) * worldSize.width + currentCell.cellColumn).GetPopulation(animalType) && at((currentCell.cellRow + 2) * worldSize.width + currentCell.cellColumn).animalWantsChild)
							weights.at(4).first += 1 * prio;
				}

				weights[0] = weights[0];
			}

			//Randomise the order for the instances where they're all bad
			std::shuffle(weights.begin(), weights.end(), std::random_device());

			//Sort in ascending order of weight
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
				//Use the value at the back of the weightings
				switch (weights.back().second)
				{
				case 0://No movement
					moved = true;
					break;

				case 1://Up 1
					if (currentCell.cellRow >= 1)
					{
						if (!newWorld[cellNum - worldSize.width].hasAnimal && (at(cellNum - worldSize.width).cellType == CellType::LAND || currentCell.cellType != CellType::LAND))
						{
							newWorld[cellNum - worldSize.width].AddAnimal(currentCell.GetAnimal());
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
					if (currentCell.cellColumn < worldSize.width - 1)
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
					if (currentCell.cellRow < worldSize.height - 1)
					{
						if (!newWorld[cellNum + worldSize.width].hasAnimal && (at(cellNum + worldSize.width).cellType == CellType::LAND || currentCell.cellType != CellType::LAND))
						{
							newWorld[cellNum + worldSize.width].AddAnimal(currentCell.GetAnimal());
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

			if (cellNum >= worldSize.width)
				if (at(cellNum - worldSize.width).hasAnimal)
					if(at(cellNum - worldSize.width).GetAnimal().type == PopulationType::PREY)
						preyPresence[0] = true;

			if (cellNum % worldSize.width > 0)
				if (at(cellNum - 1).hasAnimal)
					if (at(cellNum - 1).GetAnimal().type == PopulationType::PREY)
						preyPresence[1] = true;

			if (cellNum % worldSize.width < worldSize.width - 1)
				if (at(cellNum + 1).hasAnimal)
					if (at(cellNum + 1).GetAnimal().type == PopulationType::PREY)
						preyPresence[2] = true;

			if (cellNum < worldSize.width * (worldSize.height - 1))
				if (at(cellNum + worldSize.width).hasAnimal)
					if (at(cellNum + worldSize.width).GetAnimal().type == PopulationType::PREY)
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
					newWorld[cellNum].feedPredator(at(cellNum - worldSize.width).GetAnimal().GetMass() * 4);
					newWorld[cellNum - worldSize.width].killPrey();
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
					newWorld[cellNum].feedPredator(at(cellNum + worldSize.width).GetAnimal().GetMass() * 4);
					newWorld[cellNum + worldSize.width].killPrey();
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

			if (cellNum >= worldSize.width)
				if (at(cellNum - worldSize.width).animalWantsChild && at(cellNum - worldSize.width).hasAnimal)
					if (at(cellNum - worldSize.width).GetAnimal().type == childType)
						childBearers[0] = true;

			if (cellNum % worldSize.width > 0)
				if (at(cellNum - 1).animalWantsChild && at(cellNum - 1).hasAnimal)
					if (at(cellNum - 1).GetAnimal().type == childType)
						childBearers[1] = true;

			if (cellNum % worldSize.width < worldSize.width - 1)
				if (at(cellNum + 1).animalWantsChild && at(cellNum + 1).hasAnimal)
					if (at(cellNum + 1).GetAnimal().type == childType)
						childBearers[2] = true;

			if (cellNum < worldSize.width * (worldSize.height - 1))
				if (at(cellNum + worldSize.width).animalWantsChild && at(cellNum + worldSize.width).hasAnimal)
					if (at(cellNum + worldSize.width).GetAnimal().type == childType)
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
						if (cellNum % worldSize.width > 0)
							if (!newWorld[cellNum - 1].hasAnimal && at(cellNum - 1).cellType == CellType::LAND)
								newWorld[cellNum - 1].AddAnimal(Animal(childType));

						if (cellNum % worldSize.width < worldSize.width - 1)
							if (!newWorld[cellNum + 1].hasAnimal && at(cellNum + 1).cellType == CellType::LAND)
								newWorld[cellNum + 1].AddAnimal(Animal(childType));
					}
					else
					{
						if (cellNum % worldSize.width > 0)
							if (!newWorld[cellNum - worldSize.width - 1].hasAnimal && at(cellNum - worldSize.width - 1).cellType == CellType::LAND)
								newWorld[cellNum - worldSize.width - 1].AddAnimal(Animal(childType));

						if (cellNum % worldSize.width < worldSize.width - 1)
							if (!newWorld[cellNum - worldSize.width + 1].hasAnimal && at(cellNum - worldSize.width + 1).cellType == CellType::LAND)
								newWorld[cellNum - worldSize.width + 1].AddAnimal(Animal(childType));
					}

					break;

				case 1://Left
					if (bearer)//Children spawn next to this cell
					{
						if (cellNum >= worldSize.width)
							if (!newWorld[cellNum - worldSize.width].hasAnimal && at(cellNum - worldSize.width).cellType == CellType::LAND)
								newWorld[cellNum - worldSize.width].AddAnimal(Animal(childType));

						if (cellNum + worldSize.width < worldSize.width * (worldSize.height - 1))
							if (!newWorld[cellNum + worldSize.width].hasAnimal && at(cellNum + worldSize.width).cellType == CellType::LAND)
								newWorld[cellNum + worldSize.width].AddAnimal(Animal(childType));
					}
					else
					{
						if (cellNum >= worldSize.width)
							if (!newWorld[cellNum - 1 - worldSize.width].hasAnimal && at(cellNum - 1 - worldSize.width).cellType == CellType::LAND)
								newWorld[cellNum - 1 - worldSize.width].AddAnimal(Animal(childType));

						if (cellNum + worldSize.width < worldSize.width * (worldSize.height - 1))
							if (!newWorld[cellNum - 1 + worldSize.width].hasAnimal && at(cellNum - 1 + worldSize.width).cellType == CellType::LAND)
								newWorld[cellNum - 1 + worldSize.width].AddAnimal(Animal(childType));
					}

					break;

				case 2://Right
					if (bearer)//Children spawn next to this cell
					{
						if (cellNum >= worldSize.width)
							if (!newWorld[cellNum - worldSize.width].hasAnimal && at(cellNum - worldSize.width).cellType == CellType::LAND)
								newWorld[cellNum - worldSize.width].AddAnimal(Animal(childType));

						if (cellNum + worldSize.width < worldSize.width * (worldSize.height - 1))
							if (!newWorld[cellNum + worldSize.width].hasAnimal && at(cellNum + worldSize.width).cellType == CellType::LAND)
								newWorld[cellNum + worldSize.width].AddAnimal(Animal(childType));
					}
					else
					{
						if (cellNum >= worldSize.width)
							if (!newWorld[cellNum + 1 - worldSize.width].hasAnimal && at(cellNum + 1 - worldSize.width).cellType == CellType::LAND)
								newWorld[cellNum + 1 - worldSize.width].AddAnimal(Animal(childType));

						if (cellNum + worldSize.width < worldSize.width * (worldSize.height - 1))
							if (!newWorld[cellNum + 1 + worldSize.width].hasAnimal && at(cellNum + 1 + worldSize.width).cellType == CellType::LAND)
								newWorld[cellNum + 1 + worldSize.width].AddAnimal(Animal(childType));
					}

					break;

				case 3://Down
					if (bearer)//Children spawn next to this cell
					{
						if (cellNum % worldSize.width > 0)
							if (!newWorld[cellNum - 1].hasAnimal && at(cellNum - 1).cellType == CellType::LAND)
								newWorld[cellNum - 1].AddAnimal(Animal(childType));

						if (cellNum % worldSize.width < worldSize.width - 1)
							if (!newWorld[cellNum + 1].hasAnimal && at(cellNum + 1).cellType == CellType::LAND)
								newWorld[cellNum + 1].AddAnimal(Animal(childType));
					}
					else
					{
						if (cellNum % worldSize.width > 0)
							if (!newWorld[cellNum + worldSize.width - 1].hasAnimal && at(cellNum + worldSize.width - 1).cellType == CellType::LAND)
								newWorld[cellNum + worldSize.width - 1].AddAnimal(Animal(childType));

						if (cellNum % worldSize.width < worldSize.width - 1)
							if (!newWorld[cellNum + worldSize.width + 1].hasAnimal && at(cellNum + worldSize.width + 1).cellType == CellType::LAND)
								newWorld[cellNum + worldSize.width + 1].AddAnimal(Animal(childType));
					}

					break;
				}

				newWorld[cellNum].animalHadChild();
			}
		}

		//Update the cell
		newWorld[cellNum].Update();
	}

	//Return the back buffer
	return newWorld;
}

int EcoResilience::World::GetPopulation(PopulationType type)
{
	//Go through all the cells and see what animals are in them
	int population = 0;

	for (int cellNum = 0; cellNum < size(); cellNum++)
	{
		population += at(cellNum).GetPopulation(type);
	}

	return population;
}

void EcoResilience::World::Rain()
{
	//Add water to every cell in accordance with the generation type
	switch (worldParameters.genType)
	{
	case GenerationType::RANDOM://Random generation
		for (int row = 0; row < worldSize.height; row++)
		{
			for (int column = 0; column < worldSize.width; column++)
			{
				float water = float((rand() % 1000)) / 10000.f;
				at(row * worldSize.width + column).SetWater(water + at(row * worldSize.width + column).GetWater());
			}
		}
		break;

	case GenerationType::PERLIN://Perlin generation
		const float waterOffset = rand();

		for (int row = 0; row < worldSize.height; row++)
		{
			for (int column = 0; column < worldSize.width; column++)
			{
				float waterVec[2]{ (row + waterOffset) * worldParameters.waterMod, (column + waterOffset) * worldParameters.waterMod };
				float water = (CPerlinNoise::noise2(waterVec) + 0.5f) * 0.1f;

				at(row * worldSize.width + column).SetWater(water + at(row * worldSize.width + column).GetWater());
			}
		}
	}
}

void EcoResilience::World::UrbanDevelop()
{
	//Set the cells within the city as uninhabitable
	for(auto cell : *this)
		if(std::pow(cell.cellColumn, 2) + std::pow(cell.cellRow, 2) <= std::pow(worldSize.width / 2, 2))
			at(cell.cellRow * worldSize.width + cell.cellColumn).cellType = CellType::URBANISED;
}