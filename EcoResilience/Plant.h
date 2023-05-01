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
		void SetFire() { fire = true; };
	};
}