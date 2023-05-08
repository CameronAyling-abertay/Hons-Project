#include "Animal.h"

EcoResilience::Animal::Animal(PopulationType species) :
	type(species),
	stepsBeforeDeath(0),
	wantsDeath(false),
	wantsEat(false),
	wantsMove(false),
	wantsChild(false),
	infected(false),
	immune(false),
	burnCounter(0),
	drownCounter(0)
{
	vigor = rand() % 1000 / 100.;
	mass = vigor / 10.;
	maxAge = vigor * 40;
	age = maxAge * vigor / 15;
	stomachMax = vigor / 10.;
	stomach = stomachMax * (rand() % 1000 / 1000.f);

	switch(type)
	{
	case PopulationType::PREDATOR:
		moveCounter = 3;
		childCounter = 50;
		break;

	case PopulationType::PREY:
		moveCounter = 7;
		childCounter = 10;
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

	if(drowning)
	{
		drownCounter++;

		if (drownCounter >= static_cast<int>(vigor) + 1)
			wantsDeath = true;
	}
	else
		drownCounter = std::max(0, drownCounter - 1);

	if(burning)
	{
		mass *= 0.8f + 0.1f * vigor / 10.f;

		burnCounter++;

		if (burnCounter >= static_cast<int>(vigor) - 5)
			wantsDeath = true;
	}
	else
		burnCounter = std::max(0, burnCounter - 1);

	mass *= 1.f + 0.001f * vigor;
	vigor = std::min(10.f, vigor * 1.001f);
	stomachMax = vigor / 10.f;
	age++;
	childCounter--;

	moveCounter--;
	wantsMove = moveCounter == 0;

	switch(type)
	{
	case PopulationType::PREDATOR:
		stomach -= 0.004f / vigor;

		wantsChild = stomach >= stomachMax * 0.5f && age > 0.3f * maxAge && childCounter <= 0;

		wantsEat = stomach <= stomachMax * 0.1f && age < maxAge;

		if (moveCounter == 0)
			moveCounter = 3;

		break;

	case PopulationType::PREY:
		stomach -= 0.008f / vigor;

		wantsChild = stomach >= stomachMax * 0.5f && age > 0.3f * maxAge && childCounter <= 0;

		wantsEat = stomach <= stomachMax * 0.25f;

		if (moveCounter == 0)
			moveCounter = 7;
	}

	if (stomach <= 0)
	{
		stomach = 0;
		stepsBeforeDeath++;
		mass = std::max(mass * 0.9, 0.);
	}
	else
		stepsBeforeDeath = 0;

	if (stepsBeforeDeath == static_cast<int>(vigor))
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

	switch (type)
	{
	case PopulationType::PREDATOR:
		childCounter = 50;
		break;

	case PopulationType::PREY:
		childCounter = 10;
		break;
	}
}