#pragma once
#include <vector>
#include "Cell.h"

#define DEFAULT_SIDE 160
#define DEFAULT_MASS 2.f

namespace EcoResilience
{
	//Generation type
	enum class GenerationType { RANDOM, PERLIN };

	//Parameters
	struct EcoSize
	{
		int width = DEFAULT_SIDE;
		int height = DEFAULT_SIDE;
		float maxMass = DEFAULT_MASS;
	};

	struct EcoParameters
	{
		GenerationType genType = GenerationType::RANDOM;
		float waterMod = 0.1f;
		float altitudeMod = 0.1f;
		float plantMod = 0.1f;
		float preyMod = 0.1f;
		float predatorMod = 0.1f;
	};

	//World class
	class World : public std::vector<Cell>
	{
		//World parameters
		EcoSize worldSize;
		EcoParameters worldParameters;

		//How many steps have been taken
		int steps;
		
	public:
		//Constructor
		World();

		//Start and update
		void Generate(EcoSize size, EcoParameters parameters);
		World Update();

		//Diagnostics
		int GetWidth() const { return worldSize.width; };
		int GetHeight() const { return worldSize.height; };

		int GetPopulation(PopulationType type);

		//Disturbance events
		void Rain();
		void UrbanDevelop();
	};
};