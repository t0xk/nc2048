#include "global.h"

#ifndef NC2048_FIELD_H
#define NC2048_FIELD_H

typedef int Field[SIZE][SIZE];

void initField(Field _field);

extern void moveBlock(int *block1, int *block2);

extern void joinBlocks(int *block1, int *block2);

extern int moveFieldLeft(Field _field);

extern int moveFieldRight(Field _field);

extern int moveFieldUp(Field _field);

extern int moveFieldDown(Field _field);

extern int isFieldFull(Field _field);

extern void populateRandomBlock(Field _field);

int isFieldMovable(Field _field);

#endif //NC2048_FIELD_H
