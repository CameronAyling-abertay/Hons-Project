#pragma once
#include <vector>
#include "Animal.h"
#include "Plant.h"

namespace EcoResilience
{
	enum class CellType { WATER, LAND, URBANISED };

	class Cell
	{
		//Unique properties
		float waterLevel;

		float maxPlantMass;

		bool fire;

	public:
		int cellRow;
		int cellColumn;

		bool hasPlant;
		Plant plants;
		bool hasAnimal;
		Animal animal;

		float altitude;

		CellType cellType;

		bool flooded;
		int floodCounter;

		bool plantWantsChild;
		void plantHadChild() { plants.Reproduce(); plantWantsChild = false; }

		bool animalWantsMove;
		void animalMoved() { hasAnimal = false; }

		bool animalWantsChild;
		void animalHadChild() { animal.Reproduce(); animalWantsChild = false; }

		bool predatorWantsFood;
		void feedPredator(float biomass) { animal.Feed(biomass); predatorWantsFood = false; }
		void killPrey() { hasAnimal = false; }

		Cell(int row, int column, float maxPlantMass);
		~Cell() {};

		void Update();
		void AddPlant(Plant plant);
		void AddAnimal(Animal animal);

		float GetWater() { return waterLevel; };
		void SetWater(float newWaterLevel);

		void Flood() { flooded = true; floodCounter = 0; }

		int GetPopulation(PopulationType type);
	};
};