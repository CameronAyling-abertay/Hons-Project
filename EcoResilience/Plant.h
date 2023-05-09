#pragma once
#include <algorithm>

namespace EcoResilience
{
	class Plant
	{
		//Base stats
		float vigor;
		float mass;
		float stomach;
		float stomachMax;

		int stepsBeforeDeath;

		//Fire
		bool burnt;
		int burnCounter;

		//Drowning
		bool drowning;
	public:

		//Constructor, destructor and update
		Plant();
		Plant(float Vigor);
		void Update();

		//Flags
		bool wantsDeath;
		bool wantsChild;

		//Food
		bool wantsFood;
		void Feed(float food);
		float GetNeededFood() const { return stomachMax - stomach; };

		//Reproduction
		void Reproduce();

		//Getters
		float GetVigor() const { return vigor; }

		//Mass
		float GetMass() const { return mass; }
		void TopMass(float cap) { mass *= cap; }
		void CapMass(float cap) { mass = std::min(cap, mass); }
		void Decay(float dec) { mass -= dec; }

		//Fire
		bool fire;
		void SetFire() { if (!burnt) { fire = true; burnCounter = 0; } };
		void Extinguish() { fire = false; burnt = true; };

		//Drowning
		void Drown() { drowning = true; };
		void Resuscitate() { drowning = false; };
	};
}