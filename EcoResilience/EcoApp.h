#pragma once
#include "World.h"

#define DEFAULT_SIDE 80
#define DEFAULT_MASS 2.f

class EcoApp
{
public:
	int width;
	int height;
	EcoResilience::World* world;
	EcoResilience::World* backWorld;
	EcoResilience::GenerationType worldGenType;

	float timebank;

	bool drought;

	void Update(float deltaTime);
	void GenerateWorld(EcoResilience::GenerationType genType = EcoResilience::GenerationType::RANDOM, int worldWidth = DEFAULT_SIDE, int worldHeight = DEFAULT_SIDE, float maxCellPlantMass = DEFAULT_MASS);

	void Rain();
	void Urbanise() { world->UrbanDevelop(); }
	void Flood();
	void Drought() { drought = true; };
	void Plague();
	void Fire();

	EcoResilience::World* GetWorld() { return world; };
};