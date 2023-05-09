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
	//Non species-dependent stats
	vigor = rand() % 1000 / 100.;
	mass = vigor / 10.;
	stomachMax = vigor / 10.;
	stomach = stomachMax * (rand() % 1000 / 1000.f);

	//Species dependent stats
	switch (type)
	{
	case PopulationType::PREDATOR:
		moveCounter = rand() % 14;
		childCounter = rand() % 120;
		maxAge = vigor * 400;
		break;

	case PopulationType::PREY:
		moveCounter = rand() % 7;
		childCounter = rand() % 80;
		maxAge = vigor * 100;
		break;
	}

	age = maxAge * vigor / 15;
}

void EcoResilience::Animal::Update()
{
	//Manage plague
	//Manage Immunity
	if (immune)
	{
		immuneCounter++;
		if (immuneCounter > 50)
			immune = false;
	}

	//Manage current infection
	if (infected)
	{
		mass *= 0.9f;
		if (mass <= 0.1f)
			wantsDeath = true;

		if (rand() % 100 < 2)
			Cure();
	}

	//Manage drowning
	//Manage current state of drowning (simulates water in lungs and tiredness of body
	if(drowning)
	{
		//Animals can drown for a certain amount of time
		drownCounter++;

		//But not too much or they'll die
		if (drownCounter >= static_cast<int>(vigor) + 1)
			wantsDeath = true;
	}
	else//If the animal is not drowning, recover from the asphyxiation
		drownCounter = std::max(0, drownCounter - 1);

	//Manage burning
	//Manage how burnt the animal is
	if(burning)
	{
		mass *= 0.8f + 0.1f * vigor / 10.f;

		//Animals can withstand a certain amount of fire
		burnCounter++;

		//But not too much or theyll die
		if (burnCounter >= static_cast<int>(vigor) - 5)
			wantsDeath = true;
	}
	else//If the animal is not burning, recover from the burns
		burnCounter = std::max(0, burnCounter - 1);

	//Increase stats
	mass *= 1.f + 0.001f * vigor;
	vigor = std::min(10.f, vigor * 1.001f);
	stomachMax = vigor / 10.f;
	age++;

	//Manage counters
	childCounter--;
	moveCounter--;
	wantsMove = moveCounter == 0;
	sinceLastFeed++;

	//Species dependent actions
	switch(type)
	{
	case PopulationType::PREDATOR:
		//Deplete energy
		stomach -= 0.006f * mass * (sinceLastFeed / 5.f) / vigor;

		//Set flags
		wantsEat = stomach <= stomachMax * 0.1f && age < maxAge;
		wantsChild = stomach >= stomachMax * 0.7f && age > 0.3f * maxAge && age <= maxAge && childCounter <= 0;

		//Reset movement
		if (moveCounter == 0)
			moveCounter = 14;

		break;

	case PopulationType::PREY:
		//Deplete energy
		stomach -= 0.03f * mass * (sinceLastFeed / 5.f) / vigor;

		//Set flags
		wantsEat = stomach <= stomachMax * 0.15f && age < maxAge;
		wantsChild = stomach >= stomachMax * 0.7f && age > 0.1f * maxAge && age <= maxAge && childCounter <= 0;

		//Reset movement
		if (moveCounter == 0)
			moveCounter = 7;
	}

	//Manage hunger
	if (stomach <= 0)
	{
		stomach = 0;
		mass = std::max(mass * 0.9, 0.);
		stepsBeforeDeath++;
	}
	else if (age < maxAge)//Reset proximity to death if the animal is not too old
	{
		stepsBeforeDeath = 0;
	}
	else//The animal is too old so get closer to death
		stepsBeforeDeath++;

	//Kill the animal if it is too old or too hungry
	if (stepsBeforeDeath == static_cast<int>(vigor))
		wantsDeath = true;
}

//Feeding the animal
void EcoResilience::Animal::Feed(float food)
{
	//Reset the counter, add energy to its stomach and reset the flag
	sinceLastFeed = 0;
	stomach += food;
	wantsEat = false;
}

//Take actions that simulate reproduction
void EcoResilience::Animal::Reproduce()
{
	//Reset the flag and take the necessary energy
	wantsChild = false;
	stomach -= stomachMax * 0.7f;

	//Reset the counter
	switch (type)
	{
	case PopulationType::PREDATOR:
		childCounter = 120;
		break;

	case PopulationType::PREY:
		childCounter = 80;
		break;
	}
}