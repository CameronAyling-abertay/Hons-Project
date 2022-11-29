#pragma once

namespace EcoResilience
{
	class Plant
	{
	private:
		float stomach;
		float mass;
		float waterIntake;
		float heatThreshold;
		float vigor;
		int fruits;

		void ProduceFruit();
		void Reproduce();
		void Intake();

	public:
		Plant();
		void Update();
	};
}