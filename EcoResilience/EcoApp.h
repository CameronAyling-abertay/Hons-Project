#pragma once
#include "World.h"

class EcoApp
{
	//World parameters
	EcoResilience::GenerationType worldGenType;

	//Disturbance events
	bool drought;

public:
	//World parameters
	int width;
	int height;

	EcoResilience::World world;

	//Start and update
	EcoApp();
	void Update();
	void GenerateWorld(EcoResilience::EcoSize size, EcoResilience::EcoParameters parameters);

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