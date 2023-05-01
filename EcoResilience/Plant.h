#pragma once

namespace EcoResilience
{
	class Plant
	{
	public:
		float vigor;
		float mass;
		float stomach;
		float stomachMax;

		int maxAge;
		int age;

		int stepsBeforeDeath;
		bool wantsDeath;

		bool wantsChild;
		void Reproduce();

		Plant();
		Plant(float Vigor);
		void Update();

		bool wantsFood;
		void Feed(float food);

		bool fire;
		bool burnt;
		int burnCounter;
		void SetFire() { if (!burnt) { fire = true; burnCounter = 0; } };
		void Extinguish() { fire = false; burnt = true; };
	};
}