#pragma once
#include <vector>
#include "World.h"
#include "Animal.h"
#include "Plant.h"

namespace EcoResilience
{
	enum class PopulationType { PLANT, PREY, PREDATOR };

	class Cell
	{
	private:
		std::vector<Plant> plants;
		std::vector<Animal> animals;

		//Unique properties
		float waterLevel;
		
		World* world;
		int cellRow;
		int cellColumn;

	public:
		Cell(EcoResilience::World* world_, int row, int column);
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