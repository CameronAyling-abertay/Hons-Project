#pragma once
#include <vector>
#include "Animal.h"
#include "Plant.h"

namespace EcoResilience
{
	enum class CellType { WATER, LAND, URBANISED };

	class Cell
	{
	private:

		//Unique properties
		float waterLevel;
		
		int cellRow;
		int cellColumn;

		float maxPlantMass;

		bool fire;

	public:
		Plant* plants;
		Animal* animal;

		CellType cellType;

		bool flooded;
		int floodCounter;

		bool plantWantsChild;
		void plantHadChild() { plants->Reproduce(); plantWantsChild = false; }

		bool animalWantsMove;
		void animalMoved() { animal = NULL; }

		bool animalWantsChild;
		void animalHadChild() { animal->Reproduce(); animalWantsChild = false; }

		bool predatorWantsFood;
		void feedPredator(float biomass) { animal->Feed(biomass); predatorWantsFood = false; }
		void killPrey() { delete animal; animal = 0; }

		Cell(int row, int column, float maxPlantMass);
		~Cell() {};

		void Update();
		void AddPlant(Plant plant);
		void AddAnimal(Animal animal);

		float GetWater() { return waterLevel; };
		void SetWater(float newWaterLevel);

		int GetPopulation(PopulationType type);

		void ResetChildCount() { plantWantsChild = false; };
	};
};