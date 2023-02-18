#include "Plant.h"
#include <random>

EcoResilience::Plant::Plant()
{
	mass = float(rand() % 1000) / 1000.f;
	stomach = mass * float(rand() % 1000) / 1000.f;
	stomachMax = mass;
	waterIntake = float(rand() % 1000) / 1000.f;
	heatThreshold = float(rand() % 1000) / 1000.f;
	vigor = float(rand() % 100) / 10.f;
	stepsBeforeDeath = 0;

	wantsFood = false;
	killMe = false;
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
	wantsFood = false;
	stepsBeforeDeath = 0;
	killMe = false;
}

void EcoResilience::Plant::Update()
{
	stomach -= 0.005 * stomachMax;

	/*if (stomach > 0.6 && rand() % 100 < 60)
	{
		Reproduce();
	}
	if (stomach > 0.5f && rand() % 100 < 20)
	{
		ProduceFruit();
	}*/

	if (stomach < 0.2f * stomachMax) 
	{
		wantsFood = true;
		stepsBeforeDeath++;
	}

	if(stepsBeforeDeath == int(vigor * 10))
	{
		killMe = true;
	}
}