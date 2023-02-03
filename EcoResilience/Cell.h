#pragma once
#include <vector>
#include "Animal.h"
#include "Plant.h"

namespace EcoResilience
{
	enum class PopulationType { PLANT, PREY, PREDATOR };

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

		Cell(int row, int column);
		~Cell() {};

		void Update();
		void AddPlant(Plant plant) { plants.push_back(plant); };
		void AddAnimal(Animal animal) { animals.push_back(animal); };

		void MoveAnimal(int direction, Animal* animal);

		float GetWater() { return waterLevel; };
		void SetWater(float newWaterLevel) { waterLevel = std::min(1.f, std::max(0.f, newWaterLevel)); };

		int GetPopulation(PopulationType type);
	};
};