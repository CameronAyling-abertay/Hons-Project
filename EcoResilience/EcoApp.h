#pragma once
#include "World.h"
#include "AnimalMover.h"

#define DEFAULT_SIDE 100

class EcoApp
{
public:
	int width;
	int height;
	EcoResilience::World world;

	EcoResilience::AnimalMover animalMover;

	float timebank;
	std::vector<EcoResilience::Animal*> animals;

	void Update(float deltaTime);
	void GenerateWorld(EcoResilience::GenerationType genType = EcoResilience::GenerationType::RANDOM, int worldWidth = DEFAULT_SIDE, int worldHeight = DEFAULT_SIDE);
};