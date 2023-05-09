#pragma once
#include "Animal.h"
#include "Plant.h"

namespace EcoResilience
{
	enum class CellType { WATER, LAND, URBANISED };

	class Cell
	{
		//Unique properties
		float waterLevel;

		float maxMass;

		Plant plants;
		Animal animal;

		bool flooded;
		int floodCounter;

		float altitude;

	public:
		int cellRow;
		int cellColumn;

		bool hasPlant;
		bool hasAnimal;

		CellType cellType;

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
		~Cell() = default;

		void SetAltitude(float alt) { altitude = alt; }
		float GetAltitude() const { return altitude; }
		void Update();
		void AddPlant(Plant plant);
		void AddAnimal(Animal animal);
		Plant GetPlant() const { return plants; }
		Animal GetAnimal() const { return animal; }

		float GetWater() { return waterLevel; };
		void SetWater(float newWaterLevel);

		void Flood() { flooded = true; floodCounter = 0; }

		void InfectAnimal() { if (hasAnimal) { animal.Infect(); } }
		void SetFire() { if (hasPlant) { plants.SetFire(); } }

		int GetPopulation(PopulationType type);

		bool fire;
	};
};