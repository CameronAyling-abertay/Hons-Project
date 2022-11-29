#include "Cell.h"

EcoResilience::Cell::Cell()
{
	waterLevel = 0.5;
}

int EcoResilience::Cell::GetPopulation(PopulationType type)
{
	switch (type)
	{
	case PopulationType::PLANT:
		return plants.size();
	case PopulationType::PREY:
		return animals.size();
	}
}