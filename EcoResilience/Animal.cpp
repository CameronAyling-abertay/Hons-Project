#include "Animal.h"
#include <random>

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
	drownCounter(0),
	sinceLastFeed(0)
{
	vigor = rand() % 1000 / 100.;
	mass = vigor / 10.;

	switch (type)
	{
	case PopulationType::PREDATOR:
		moveCounter = 14;
		childCounter = 120;
		maxAge = vigor * 400;
		break;

	case PopulationType::PREY:
		moveCounter = 7;
		childCounter = 60;
		maxAge = vigor * 100;
		break;
	}

	age = maxAge * vigor / 15;
	stomachMax = vigor / 10.;
	stomach = stomachMax * (rand() % 1000 / 1000.f);
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

		if (rand() % 1000 < 5)
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

	sinceLastFeed++;

	switch(type)
	{
	case PopulationType::PREDATOR:
		stomach -= 0.006f * mass * (sinceLastFeed / 5.f) / vigor;

		wantsEat = stomach <= stomachMax * 0.1f && age < maxAge;
		wantsChild = stomach >= stomachMax * 0.7f && age > 0.3f * maxAge && age <= maxAge && childCounter <= 0;

		if (moveCounter == 0)
			moveCounter = 14;

		break;

	case PopulationType::PREY:
		stomach -= 0.03f * mass * (sinceLastFeed / 5.f) / vigor;

		wantsEat = stomach <= stomachMax * 0.15f && age < maxAge;
		wantsChild = stomach >= stomachMax * 0.7f && age > 0.1f * maxAge && age <= maxAge && childCounter <= 0;

		if (moveCounter == 0)
			moveCounter = 7;
	}

	if (stomach <= 0)
	{
		stomach = 0;
		mass = std::max(mass * 0.9, 0.);
		stepsBeforeDeath++;
	}
	else if (age < maxAge)
	{
		stepsBeforeDeath = 0;
	}
	else
		stepsBeforeDeath++;

	if (stepsBeforeDeath == static_cast<int>(vigor))
	{
		wantsDeath = true;
	}
}

void EcoResilience::Animal::Feed(float food)
{
	sinceLastFeed = 0;
	stomach += food;
	wantsEat = false;
}

void EcoResilience::Animal::Reproduce()
{
	wantsChild = false;
	stomach -= stomachMax * 0.7f;

	switch (type)
	{
	case PopulationType::PREDATOR:
		childCounter = 120;
		break;

	case PopulationType::PREY:
		childCounter = 60;
		break;
	}
}