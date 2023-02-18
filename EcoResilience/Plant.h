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
		void Reproduce();

	public:
		float mass;
		float stomach;
		float stomachMax;

		Plant();
		void Update();

		bool wantsFood;
		void Feed(float food);
	};
}