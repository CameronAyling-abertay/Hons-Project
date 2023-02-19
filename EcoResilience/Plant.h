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
		void Kill() { wantsDeath = true; };

		bool wantsChild;
		void Reproduce();

		Plant();
		void Update();

		bool wantsFood;
		void Feed(float food);
	};
}