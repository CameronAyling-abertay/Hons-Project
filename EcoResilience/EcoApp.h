#pragma once
#include "World.h"

#define DEFAULT_SIDE 160
#define DEFAULT_MASS 2.f

class EcoApp
{
	//World parameters
	bool drought;
	EcoResilience::GenerationType worldGenType;

	//Disturbance events
	float timebank;

public:
	//World parameters
	int width;
	int height;

	EcoResilience::World world;

	//Start and update
	EcoApp();
	void Update();
	void GenerateWorld(EcoResilience::GenerationType genType = EcoResilience::GenerationType::RANDOM, int worldWidth = DEFAULT_SIDE, int worldHeight = DEFAULT_SIDE, float maxCellMass = DEFAULT_MASS);

	//Disturbance events
	void Rain();
	void Urbanise() { world.UrbanDevelop(); }
	void Flood();
	void Drought() { drought = true; };
	void Plague();
	void Fire();

	//Getters
	bool GetDrought() const { return drought; }
};