#include "global.h"

#ifndef NC2048_RANDOM_H
#define NC2048_RANDOM_H

extern void initRandom();
extern int randInt(int upperLimit);
#define randFieldCoordinate() randInt(SIZE - 1)

#endif //NC2048_RANDOM_H
