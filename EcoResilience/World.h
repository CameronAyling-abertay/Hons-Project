#pragma once
#include <vector>
#include "Cell.h"

namespace EcoResilience
{
	class World : public std::vector<Cell*>
	{
	private:
		int width;
		int height;

	public:
		World(int width, int height);
		~World();

		void Update();

		int GetWidth() { return width; };
		int GetHeight() { return height; };
	};
};