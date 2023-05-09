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
		float altitude;
		float maxMass;

		//Contained objects
		Plant plants;
		Animal animal;

		//Disturbance Effects
		float preFlood;
		bool flooded;

	public:
		//Location
		int cellRow;
		int cellColumn;

		//Flags that act in the same way as pointers for presence verification
		bool hasPlant;
		bool hasAnimal;

		//The landscape within the cell
		CellType cellType;

		//Intermediary functions that allow the world object to communicate with the contained objects without direct reference
		bool plantWantsChild;
		void plantHadChild() { plants.Reproduce(); plantWantsChild = false; }

		bool animalWantsMove;
		void animalMoved() { hasAnimal = false; }

		bool animalWantsChild;
		void animalHadChild() { animal.Reproduce(); animalWantsChild = false; }

		bool predatorWantsFood;
		void feedPredator(float biomass) { animal.Feed(biomass); predatorWantsFood = false; }
		void killPrey() { hasAnimal = false; }

		//Constructor/Destructor
		Cell(int row, int column, float maxPlantMass);
		~Cell() = default;
		void Update();

		//Getters and Setters
		void SetAltitude(float alt) { altitude = alt; }
		float GetAltitude() const { return altitude; }
		float GetWater() { return waterLevel; };
		void SetWater(float newWaterLevel);
		void AddPlant(Plant plant);
		void AddAnimal(Animal animal);
		Plant GetPlant() const { return plants; }
		Animal GetAnimal() const { return animal; }

		//Disturbance effects
		void Flood() { if (!flooded) { flooded = true; preFlood = waterLevel; } }

		void InfectAnimal() { if (hasAnimal) { animal.Infect(); } }

		void SetFire() { if (hasPlant) { plants.SetFire(); } }
		bool fire;

		//Diagnostics
		int GetPopulation(PopulationType type);
	};
};