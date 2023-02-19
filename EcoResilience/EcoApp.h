#pragma once
#include "World.h"

#define DEFAULT_SIDE 40
#define DEFAULT_MASS 2.f

class EcoApp
{
public:
	int width;
	int height;
	EcoResilience::World world;
	EcoResilience::GenerationType worldGenType;

	float timebank;

	void Update(float deltaTime);
	void GenerateWorld(EcoResilience::GenerationType genType = EcoResilience::GenerationType::RANDOM, int worldWidth = DEFAULT_SIDE, int worldHeight = DEFAULT_SIDE, float maxCellPlantMass = DEFAULT_MASS);

	void Rain() { world.Rain(); };
};