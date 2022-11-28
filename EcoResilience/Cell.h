#pragma once
#include <vector>

namespace EcoResilience
{
	class Cell
	{
	private:
		//Animal set
		//Plant set

		//Unique properties
		float waterLevel;
	public:
		Cell();
		~Cell() {};

		void Update() {};

		float GetWater() { return waterLevel; };

		void SetWater(float newWaterLevel) { waterLevel = std::min(1.f, std::max(0.f, newWaterLevel)); };
	};
};