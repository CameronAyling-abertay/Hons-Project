#include "NewPlant.h"

EcoResilience::NewPlant::NewPlant()
{
	mass *= 0.01f;
	stomachMax = mass;
	stomach = stomachMax;
}