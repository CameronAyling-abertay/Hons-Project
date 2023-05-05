#pragma once
#include <random>

namespace EcoResilience
{
	enum class PopulationType { PREY, PREDATOR };

	class Animal
	{
	public:
		void Reproduce();

		float stomach;
		float stomachMax;
		float mass;
		float vigor;

		Animal(PopulationType species = PopulationType::PREY);
		~Animal() {};

		PopulationType type;

		int stepsBeforeDeath;
		bool wantsDeath;

		bool wantsEat;
		
		bool wantsMove;
		int moveCounter;

		bool wantsChild;


		void Update();

		void Feed(float food);

		bool infected;
		bool immune;
		bool immuneCounter;
		void Infect() { if (!immune) { infected = true; immuneCounter = 0; } };
		void Cure() { infected = false; immune = true; }
	};
}