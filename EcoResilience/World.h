#pragma once
#include <vector>
#include "Cell.h"

namespace EcoResilience
{
	//Generation type
	enum class GenerationType { RANDOM, PERLIN };

	class World : public std::vector<Cell>
	{
		//World parameters
		int width;
		int height;
		
		float maxCellPlantMass;

		GenerationType genType;

		//How many steps have been taken
		int steps;

	public:
		//Constructor
		World();

		//Start and update
		void Generate(int width, int height, GenerationType type, float maxCellPlantMass);
		World Update();

		//Diagnostics
		int GetWidth() { return width; };
		int GetHeight() { return height; };

		int GetPopulation(PopulationType type);

		//Disturbance events
		void Rain();
		void UrbanDevelop();
	};
};