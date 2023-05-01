#pragma once
#include <random>

namespace EcoResilience
{
	enum class PopulationType { PREY, PREDATOR };

	class Animal
	{
	public:
		void Move();
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

		bool infected;

		void Update();
		void Survey(bool ideal) { wantsMove = !ideal; };

		void Feed(float food);

		void Infect() { infected = true; };
	};
}