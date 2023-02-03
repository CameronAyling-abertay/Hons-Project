#pragma once
#include "Animal.h"
#include "World.h"
#include <vector>

namespace EcoResilience
{
	class AnimalMover
	{
	public:
		void moveAnimals();

		std::vector<Animal*> animals;
		World* world;
	};
}