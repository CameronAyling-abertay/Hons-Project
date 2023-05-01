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
		float speed;

		Animal(PopulationType species);
		~Animal() {};

		PopulationType type;

		int stepsBeforeDeath;
		bool wantsDeath;

		bool wantsEat;
		
		bool wantsMove;

		bool wantsChild;


		void Update();
		void Survey(bool ideal) { wantsMove = !ideal; };

		void Feed(float food);

		bool infected;
		bool immune;
		bool immuneCounter;
		void Infect() { if (!immune) { infected = true; immuneCounter = 0; } };
		void Cure() { infected = false; immune = true; }
	};
}