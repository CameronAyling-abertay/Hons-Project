#include "Plant.h"
#include <random>

EcoResilience::Plant::Plant()
{
	mass = float(rand() % 1000) / 1000.f;
	stomach = mass * float(rand() % 1000) / 1000.f;
	stomachMax = mass;

	vigor = float(rand() % 9000) / 1000.f + 1.f;

	maxAge = pow(static_cast<int>(vigor), 5);
	age = 0;
	stepsBeforeDeath = 0;

	wantsFood = false;
	wantsChild = false;
	wantsDeath = false;
}

void EcoResilience::Plant::Reproduce()
{
	stomach -= 0.5f * stomachMax;
	wantsChild = false;
}

void EcoResilience::Plant::Feed(float food)
{
	stomach += food;
	wantsFood = false;
	stepsBeforeDeath = 0;
	wantsDeath = false;
}

void EcoResilience::Plant::Update()
{
	stomach -= 0.005f * stomachMax;
	mass *= 1.f + (vigor * 0.001f);
	vigor += (9.f - vigor) * 0.001f;
	age++;

	if (stomach > 0.6f * stomachMax && rand() % 1000 < (50.f * 9.f / vigor))
	{
		wantsChild = true;
	}

	if (stomach < 0.2f * stomachMax && age < maxAge) 
	{
		wantsFood = true;
		stepsBeforeDeath++;
	}

	if(stepsBeforeDeath == static_cast<int>(vigor) || stomach < 0)
	{
		Kill();
	}
}