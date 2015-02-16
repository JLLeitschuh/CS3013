#include "Random.h"


long getRandomBetween(long min, long max){
  long randomValue = random();
  long range = max - min;
  long output = (randomValue % range) + min;
  return output;
}
