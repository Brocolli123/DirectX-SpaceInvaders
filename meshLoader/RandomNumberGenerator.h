#ifndef  RNG_H
#define RNG_H

class RandomNumberGenerator
{
public:
  void seed() const;
  int getRandomValue(int) const;
  int getRandomValue(int min, int max) const;
  float getRandomValue(float min, float max) const;
};

#endif // ! RNG_H
