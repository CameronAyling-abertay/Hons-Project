#include "Animal.h"

EcoResilience::Animal::Animal(PopulationType species) :
	type(species),
	stepsBeforeDeath(0),
	wantsDeath(false),
	wantsEat(false),
	wantsMove(false),
	wantsChild(false),
	infected(false),
	immune(false)
{
	vigor = rand() % 1000 / 100.;
	mass = vigor / 10.;
	stomachMax = vigor / 10.;
	stomach = stomachMax * (rand() % 1000 / 1000.f);

	switch(type)
	{
	case PopulationType::PREDATOR:
		moveCounter = 3;
		break;

	case PopulationType::PREY:
		moveCounter = 5;
		break;
	}
}

void EcoResilience::Animal::Update()
{
	if (immune)
	{
		immuneCounter++;
		if (immuneCounter > 50)
			immune = false;
	}

	if (infected)
	{
		mass *= 0.9f;
		if (mass <= 0.1f)
			wantsDeath = true;

		if (rand() % 100 < 2)
			Cure();
	}

	mass *= 1.001f;
	vigor = std::min(10.f, vigor * 1.001f);
	stomachMax = vigor / 10.;

	moveCounter--;
	wantsMove = moveCounter == 0;

	switch(type)
	{
	case PopulationType::PREDATOR:
		stomach -= 0.001f / vigor;

		wantsChild = stomach >= stomachMax * 0.5f;

		wantsEat = stomach <= stomachMax * 0.1f;

		if (moveCounter == 0)
			moveCounter = 3;

		break;

	case PopulationType::PREY:
		stomach -= 0.003f / vigor;

		wantsChild = stomach >= stomachMax * 0.5f;

		wantsEat = stomach <= stomachMax * 0.2f;

		if (moveCounter == 0)
			moveCounter = 5;
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