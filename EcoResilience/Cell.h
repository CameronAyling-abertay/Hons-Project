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
		std::vector<Plant> plants;
		std::vector<Animal> animals;

		//Unique properties
		float waterLevel;
		
	public:
		Cell();
		~Cell() {};

		void Update() {};
		void AddPlant() { plants.push_back(Plant()); };

		float GetWater() { return waterLevel; };
		void SetWater(float newWaterLevel) { waterLevel = std::min(1.f, std::max(0.f, newWaterLevel)); };

		int GetPopulation(PopulationType type);
	};
};