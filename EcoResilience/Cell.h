#pragma once
#include <vector>
#include "Animal.h"
#include "Plant.h"

namespace EcoResilience
{
	enum class PopulationType { PLANT, PREY, PREDATOR };
	enum class CellType { WATER, LAND };

	class Cell
	{
	private:

		//Unique properties
		float waterLevel;
		
		int cellRow;
		int cellColumn;

	public:
		std::vector<Plant> plants;
		std::vector<Animal> animals;

		CellType cellType;

		int desiredChildCount;

		Cell(int row, int column);
		~Cell() {};

		void Update();
		void AddPlant(Plant plant) { plants.push_back(plant); };
		void AddAnimal(Animal animal) { animals.push_back(animal); };

		float GetWater() { return waterLevel; };
		void SetWater(float newWaterLevel) { waterLevel = std::min(1.f, std::max(0.f, newWaterLevel)); (waterLevel * 255.f > 150.f) ? cellType = CellType::WATER : cellType = CellType::LAND; };

		int GetPopulation(PopulationType type);

		void ResetChildCount() { desiredChildCount = 0; };
	};
};