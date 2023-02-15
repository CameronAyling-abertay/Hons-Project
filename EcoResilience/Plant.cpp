#include "Plant.h"
#include <random>

EcoResilience::Plant::Plant()
{
	stomach = float(rand() % 10000) / 10000.f;
	mass = float(rand() % 1000) / 100.f;
	waterIntake = float(rand() % 1000) / 1000.f;
	heatThreshold = float(rand() % 1000) / 1000.f;
	vigor = float(rand() % 100) / 10.f;
}

void EcoResilience::Plant::ProduceFruit()
{
	stomach -= 0.3;
	fruits += 10;
}

void EcoResilience::Plant::Reproduce()
{
	stomach -= 0.5;
}

void EcoResilience::Plant::Feed(float food)
{
	stomach += food;
}

void EcoResilience::Plant::Update()
{
	stomach -= 0.005 * mass;

	if (stomach > 0.6 && rand() % 100 < 60)
	{
		Reproduce();
	}
	if (stomach > 0.5f && rand() % 100 < 20)
	{
		ProduceFruit();
	}
	if (stomach < 0.2f) 
	{
		wantsFood = true;
	}
}