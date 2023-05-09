#pragma once
#include <algorithm>

namespace EcoResilience
{
	enum class PopulationType { PREY, PREDATOR };

	class Animal
	{
		//General data
		int sinceLastFeed;
		float stomach;
		float stomachMax;
		float mass;
		float vigor;

		//Closeness to death
		int stepsBeforeDeath;

		int age;
		int maxAge;

		//Countdown to well it can have a child
		int childCounter;

		//Countdown to when it can move
		int moveCounter;

		//Immunity to plague
		bool immune;
		bool immuneCounter;

		//Fire
		bool burning;
		int burnCounter;

		//Drowning
		bool drowning;
		int drownCounter;

	public:
		//Constructor and destructor
		Animal(PopulationType species = PopulationType::PREY);
		~Animal() = default;

		void Update();

		//The species
		PopulationType type;

		//Flags
		bool wantsChild;
		bool wantsDeath;
		bool wantsEat;
		bool wantsMove;

		//Reproduction
		void Reproduce();

		//Mass Management
		float GetMass() const { return mass; }
		void CapMass(float cap) { mass = std::min(cap, mass); }

		//Food
		void Feed(float food);
		float GetNeededFood() const { return stomachMax - stomach; }
		float GetFullness() const { return stomach; }

		//Plague
		bool infected;
		void Infect() { if (!immune) { infected = true; immuneCounter = 0; } };
		void Cure() { infected = false; immune = true; }

		//Fire
		void Burn() { burning = true; };
		void RecoverBurn() { burning = false; };

		//Drowning
		void Drown() { drowning = true; };
		void Resuscitate() { drowning = false; };
	};
}