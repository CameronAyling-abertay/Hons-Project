#pragma once

namespace EcoResilience
{
	class Plant
	{
	private:
		float waterIntake;
		float heatThreshold;
		float vigor;
		int fruits;

		void ProduceFruit();

	public:
		float mass;
		float stomach;
		float stomachMax;

		int stepsBeforeDeath;
		bool wantsDeath;

		bool wantsChild;
		void Reproduce();

		Plant();
		void Update();

		bool wantsFood;
		void Feed(float food);
	};
}