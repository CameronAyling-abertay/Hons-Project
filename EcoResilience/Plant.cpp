#include "Plant.h"
#include <random>

EcoResilience::Plant::Plant() :
	wantsDeath(false),
	wantsChild(false),
	wantsFood(false),
	fire(false),
	burnt(false),
	drowning(false)
{
	vigor = float(rand() % 1000) / 100.f;

	mass = vigor / 10.f;
	stomachMax = vigor * 0.05f;
	stomach = stomachMax * float(rand() % 1000) / 1000.f;

	maxAge = vigor * 500;
	age = maxAge * vigor / 15 ;
	stepsBeforeDeath = 0;
}

EcoResilience::Plant::Plant(float newVigor) :
	wantsDeath(false),
	wantsChild(false),
	wantsFood(false),
	fire(false),
	burnt(false),
	drowning(false)
{
	vigor = newVigor;

	mass = vigor / 10.f;
	stomachMax = vigor * 0.05f;
	stomach = stomachMax * (rand() % 1000 / 1000.f);

	maxAge = 10000;
	age = 0;
	stepsBeforeDeath = 0;
}

void EcoResilience::Plant::Reproduce()
{
	stomach -= 0.3f * stomachMax;
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
	if (burnt)
	{
		burnCounter++;
		if (burnCounter > 50)
			burnt = false;
	}

	if (fire)
	{
		mass *= 0.8f;
		if (mass <= 0.1f - vigor / 10.f)
			wantsDeath = true;

		if (rand() % 100 < 20)
			Extinguish();
	}

	if (drowning)
	{
		mass *= std::min(1.f, 0.9f + 0.1f * vigor / 10.f);

		if (mass <= 0.1f - vigor / 10.f)
			wantsDeath = true;
	}

	stomach -= 0.005f * stomachMax;
	mass *= 1.001f;
	vigor = std::min(10.f, vigor * 1.001f);
	stomachMax = vigor * 0.05f;
	age++;

	wantsChild = stomach > 0.4f * stomachMax && age > 0.3f * maxAge;

	wantsFood = stomach < 0.2f * stomachMax && age < maxAge;

	if (stomach <= 0)
	{
		stomach = 0;
		stepsBeforeDeath++;
		mass = std::max(mass * 0.9, 0.);
	}
	else
		stepsBeforeDeath = 0;

	if(stepsBeforeDeath == static_cast<int>(vigor) + 5)
	{
		wantsDeath = true;
	}
}