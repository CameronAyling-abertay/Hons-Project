#pragma once
#include <random>

namespace EcoResilience
{
	class Animal
	{
	private:
		void Move();
		void Reproduce();

		float stomach;
		float mass;
		float vigor;
		float speed;

	public:
		Animal();
		~Animal() {};

		void Update();
	};
}