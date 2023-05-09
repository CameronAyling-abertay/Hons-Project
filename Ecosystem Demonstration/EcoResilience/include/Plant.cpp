#include "Plant.h"
#include <random>

EcoResilience::Plant::Plant() :
	wantsDeath(false),
	wantsChild(false),
	wantsFood(false),
	fire(false),
	burnt(false),
	drowning(false),
	stepsBeforeDeath(0)
{
	//Set up base stats that rely on randomness or other stats
	vigor = float(rand() % 1000) / 100.f;

	mass = vigor / 10.f;
	stomachMax = vigor * 0.05f;
	stomach = stomachMax * float(rand() % 1000) / 1000.f;
}

EcoResilience::Plant::Plant(float newVigor) :
	wantsDeath(false),
	wantsChild(false),
	wantsFood(false),
	fire(false),
	burnt(false),
	drowning(false),
	stepsBeforeDeath(0),
	vigor(newVigor)
{
	//Set up base stats that rely on randomness or other stats
	mass = vigor / 10.f;
	stomachMax = vigor * 0.05f;
	stomach = stomachMax * (rand() % 1000 / 1000.f);
}

//Simulate the effects of reproduction
void EcoResilience::Plant::Reproduce()
{
	stomach -= 0.3f * stomachMax;
	wantsChild = false;
}

//Feeding the animal
void EcoResilience::Plant::Feed(float food)
{
	//Reset the flags and counter, add energy to its stomach
	stomach += food;
	wantsFood = false;
	stepsBeforeDeath = 0;
	wantsDeath = false;
}

void EcoResilience::Plant::Update()
{
	//If the plant is currently unable to be set alight, count down to when it can
	if (burnt)
	{
		burnCounter++;
		if (burnCounter > 200)
			burnt = false;
	}

	//if the plant is currently on fire
	if (fire)
	{
		//Decay the plant an amount
		mass *= 0.8f;

		//Kill the plant if it is below a mass threshold
		if (mass <= 0.1f - (10 - vigor) / 10.f)
			wantsDeath = true;

		//Give it a chance to extinguish
		if (rand() % 100 < 20)
			Extinguish();
	}

	//If the plant is currently underwater
	if (drowning)
	{
		//Wash away some of the plant mass
		mass *= std::min(1.f, 0.9f + 0.1f * vigor / 10.f);

		//Kill the plant if it has been sufficiently washed away
		if (mass <= 0.1f - (10 - vigor) / 10.f)
			wantsDeath = true;
	}

	//Increase base stats for growth
	stomach -= 0.02f * stomachMax;
	mass *= 1 + 0.001f * vigor;
	vigor = std::min(10.f, vigor * 1.001f);
	stomachMax = vigor * 0.05f;

	//Set flags
	wantsChild = stomach > 0.4f * stomachMax;
	wantsFood = stomach < 0.2f * stomachMax;

	//If the plant has nothing in its stomach then place it closer to death and decay its mass some
	if (stomach <= 0)
	{
		stomach = 0;
		stepsBeforeDeath++;
		mass = std::max(mass * 0.9, 0.);
	}
	else//Otherwise, reset its death counter to zero
		stepsBeforeDeath = 0;

	//If the plant is sufficiently close to death, kill it
	if(stepsBeforeDeath == static_cast<int>(vigor) + 5)
		wantsDeath = true;
}