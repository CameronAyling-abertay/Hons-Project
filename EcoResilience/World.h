#pragma once
#include <vector>
#include "Cell.h"

namespace EcoResilience
{
	enum class GenerationType { RANDOM, PERLIN };

	class World : public std::vector<Cell>
	{
	private:
		int width;
		int height;
		
		float maxCellPlantMass;

		GenerationType genType;

		int steps;

	public:
		World();

		void Generate(int width, int height, GenerationType type, float maxCellPlantMass);
		World Update();

		int GetWidth() { return width; };
		int GetHeight() { return height; };

		int GetPopulation(PopulationType type);

		void Rain();

		void UrbanDevelop();
	};
};