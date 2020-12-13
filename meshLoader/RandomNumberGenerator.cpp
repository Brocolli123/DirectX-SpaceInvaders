#include "RandomNumberGenerator.h"
#include <ctime>	//for time used in random number routines
#include <stdlib.h>

void RandomNumberGenerator::seed() const
//seed the random number generator from current system time
//so that the numbers will be different every time
{
  srand(static_cast<unsigned>(time(0)));
}
int RandomNumberGenerator::getRandomValue(int max) const
//produce a random number in range [1..max]
{
  //return (rand() % max) + 1;
	return (rand() % max+1);		//inclusive of 0 and max (0-2)
}
int RandomNumberGenerator::getRandomValue(int min, int max) const {
  //produce a random number in a set range [min..max]
  return (rand() % (max + 1 - min) + min);    
}
float RandomNumberGenerator::getRandomValue(float min, float max) const {
  return (min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min))));
}