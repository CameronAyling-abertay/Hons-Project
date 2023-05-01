#include "Animal.h"

EcoResilience::Animal::Animal(PopulationType species) :
	type(species),
	wantsEat(false),
	wantsDeath(false),
	wantsMove(false),
	stepsBeforeDeath(0),
	infected(false)
{
	vigor = rand() % 1000 / 100.;
	mass = vigor / 10.;
	stomachMax = vigor / 10.;
	stomach = stomachMax * (rand() % 1000 / 1000.f);

	switch(type)
	{
	case PopulationType::PREDATOR:
		speed = 3;
		break;

	case PopulationType::PREY:
		speed = 1;
		break;

	}

}

void EcoResilience::Animal::Update()
{
	mass *= 1.001f;
	vigor = std::min(10.f, vigor * 1.001f);
	stomachMax = vigor / 10.;

	switch(type)
	{
	case PopulationType::PREDATOR:
		stomach -= 0.001f / vigor;

		wantsChild = stomach >= stomachMax * 0.5f;

		wantsEat = stomach <= stomachMax * 0.1f;

		break;

	case PopulationType::PREY:
		stomach -= 0.003f / vigor;

		wantsChild = stomach >= stomachMax * 0.5f;

		wantsEat = stomach <= stomachMax * 0.2f;
	}

	if (stomach <= 0)
	{
		stomach = 0;
		stepsBeforeDeath++;
		mass = std::max(mass * 0.9, 0.);
	}
	else
		stepsBeforeDeath = 0;

	if (stepsBeforeDeath == static_cast<int>(vigor) + 5)
	{
		wantsDeath = true;
	}
}

void EcoResilience::Animal::Feed(float food)
{
	stomach += food;
	wantsEat = false;
}

void EcoResilience::Animal::Reproduce()
{
	wantsChild = false;
	stomach -= stomachMax * 0.5f;
}