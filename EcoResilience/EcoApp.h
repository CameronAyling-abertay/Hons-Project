#pragma once
#include "World.h"

#define DEFAULT_SIDE 40

class EcoApp
{
public:
	int width;
	int height;
	EcoResilience::World world;
	EcoResilience::GenerationType worldGenType;

	float timebank;

	void Update(float deltaTime);
	void GenerateWorld(EcoResilience::GenerationType genType = EcoResilience::GenerationType::RANDOM, int worldWidth = DEFAULT_SIDE, int worldHeight = DEFAULT_SIDE);

	void Rain() { world.Rain(worldGenType); };
};