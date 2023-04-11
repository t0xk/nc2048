#include "field.h"
#include "score.h"
#include "random.h"

/**
 * Sets all the values in the field to 0, effectively clearing it of all blocks.
 * @param _field
 */
void emptyField(Field _field) {
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            _field[i][j] = 0;
}

/**
 * 'Initializes' a new field.
 *  - Empties it
 *  - Spawns 2 blocks in random location
 * @param _field
 */
void initField(Field _field) {
#define FIELD_SIZE (SIZE * SIZE * sizeof(int))
    emptyField(_field);

    populateRandomBlock(_field);
    populateRandomBlock(_field);
#undef FIELD_SIZE
}

/**
 * Moves and joins(where applicable) field blocks to the <i>left</i>.
 * @param _field Field we're iterating.
 */
int moveFieldLeft(Field _field) {
#define BLOCK_Y_START 0
#define BLOCK_X_START 1
#define FIRST_BLOCK 0
    int moveCount = 0;

    // For each row in the field
    for (int blockY = BLOCK_Y_START; blockY < SIZE; blockY++) {
        int sBlockMin = FIRST_BLOCK;

        // For each block in the row
        for (int blockX = BLOCK_X_START; blockX < SIZE; blockX++) {
            // Pointer to the integer value in [y][x]
            int *block = &_field[blockY][blockX];

            // If the block is empty we can't move it
            if (*block == 0)
                continue;
            // Search for the left-most open spot. Start at 0 + offset.
            // Offset value:
            //  - If a block has been placed after another one, the offset is updated to the left one.
            //  - If two block are joined the offset becomes the location of the joint block
            for (int sBlockX = sBlockMin; sBlockX < blockX; sBlockX++) {
                int *sBlock = &_field[blockY][sBlockX];

                // The block is empty:
                // - Move value of block to sBlock
                // - Clear block
                if (*sBlock == 0) {
                    moveBlock(block, sBlock);

                    moveCount++;
                    sBlockMin = sBlockX;
                    break;
                }

                // Blocks are identical and should be joined
                if (*sBlock == *block) {
                    joinBlocks(block, sBlock);

                    moveCount++;
                    sBlockMin = sBlockX + 1;
                    break;
                }

                // Blocks were not moved. If this is the final element of the loop and nothing happened,
                // we act like we moved into this spot.
                if (sBlockX == (blockX - 1)) {
                    sBlockMin = sBlockX + 1;
                    break;
                }

            }
        }
    }

    return moveCount;
#undef BLOCK_Y_START
#undef BLOCK_X_START
#undef LAST_BLOCK
}

/**
 * Moves and joins(where applicable) field blocks to the <i>right</i>.
 * @param _field Field we're iterating.
 */
int moveFieldRight(Field _field) {
#define BLOCK_Y_START 0
#define BLOCK_X_START (SIZE - 2)
#define LAST_BLOCK (SIZE - 1)
    int moveCount = 0;

    for (int blockY = BLOCK_Y_START; blockY < SIZE; blockY++) {
        int sBlockMax = LAST_BLOCK;

        for (int blockX = BLOCK_X_START; blockX >= 0; blockX--) {
            int *block = &_field[blockY][blockX];

            if (*block == 0)
                continue;

            for (int sBlockX = sBlockMax; sBlockX > blockX; sBlockX--) {
                int *sBlock = &_field[blockY][sBlockX];

                if (*sBlock == 0) {
                    moveBlock(block, sBlock);

                    moveCount++;
                    sBlockMax = sBlockX;
                    break;
                }

                if (*sBlock == *block) {
                    joinBlocks(block, sBlock);

                    moveCount++;
                    sBlockMax = sBlockX - 1;
                    break;
                }

                if (sBlockX == (blockX + 1)) {
                    sBlockMax = sBlockX - 1;
                    break;
                }
            }
        }
    }

    return moveCount;
#undef BLOCK_Y_START
#undef BLOCK_X_START
#undef LAST_BLOCK
}

/**
 * Moves and joins(where applicable) field blocks <i>up</i>.
 * @param _field Field we're iterating.
 */
int moveFieldUp(Field _field) {
#define BLOCK_X_START 0
#define BLOCK_Y_START 1
#define FIRST_BLOCK 0
    int moveCount = 0;

    // For each row in the field
    for (int blockX = BLOCK_X_START; blockX < SIZE; blockX++) {
        int sBlockMin = FIRST_BLOCK;

        // For each block in the row
        for (int blockY = BLOCK_Y_START; blockY < SIZE; blockY++) {
            // Pointer to the integer value in [y][x]
            int *block = &_field[blockY][blockX];

            // If the block is empty we can't move it
            if (*block == 0)
                continue;
            // Search for the left-most open spot. Start at 0 + offset.
            // Offset value:
            //  - If a block has been placed after another one, the offset is updated to the left one.
            //  - If two block are joined the offset becomes the location of the joint block
            for (int sBlockY = sBlockMin; sBlockY < blockY; sBlockY++) {
                int *sBlock = &_field[sBlockY][blockX];

                // The block is empty:
                // - Move value of block to sBlock
                // - Clear block
                if (*sBlock == 0) {
                    moveBlock(block, sBlock);

                    moveCount++;
                    sBlockMin = sBlockY;
                    break;
                }

                // Blocks are identical and should be joined
                if (*sBlock == *block) {
                    joinBlocks(block, sBlock);

                    moveCount++;
                    sBlockMin = sBlockY + 1;
                    break;
                }

                // Blocks were not moved. If this is the final element of the loop and nothing happened,
                // we act like we moved into this spot.
                if (sBlockY == (blockY - 1)) {
                    sBlockMin = sBlockY + 1;
                    break;
                }

            }
        }
    }

    return moveCount;
#undef BLOCK_Y_START
#undef BLOCK_X_START
#undef LAST_BLOCK
}

/**
 * Moves and joins(where applicable) field blocks <i>down</i>.
 * @param _field Field we're iterating.
 */
int moveFieldDown(Field _field) {
#define BLOCK_X_START 0
#define BLOCK_Y_START (SIZE - 2)
#define LAST_BLOCK (SIZE - 1)
    int moveCount = 0;

    for (int blockX = BLOCK_X_START; blockX < SIZE; blockX++) {
        int sBlockMax = LAST_BLOCK;

        for (int blockY = BLOCK_Y_START; blockY >= 0; blockY--) {
            int *block = &_field[blockY][blockX];

            if (*block == 0)
                continue;

            for (int sBlockY = sBlockMax; sBlockY > blockY; sBlockY--) {
                int *sBlock = &_field[sBlockY][blockX];

                if (*sBlock == 0) {
                    moveBlock(block, sBlock);;

                    moveCount++;
                    sBlockMax = sBlockY;
                    break;
                }

                if (*sBlock == *block) {
                    joinBlocks(block, sBlock);

                    moveCount++;
                    sBlockMax = sBlockY - 1;
                    break;
                }

                if (sBlockY == (blockY + 1)) {
                    sBlockMax = sBlockY - 1;
                    break;
                }
            }
        }
    }

    return moveCount;
#undef BLOCK_Y_START
#undef BLOCK_X_START
#undef LAST_BLOCK
}

/**
 * Populates a random block on the field.
 *  TODO: improve the efficiency, when the amount of free blocks grows
 * @param _field
 */
void populateRandomBlock(Field _field) {
    int randY;
    int randX;

    do {
        randX = randFieldCoordinate();
        randY = randFieldCoordinate();
    } while (_field[randY][randX] != 0);

    int rand = randInt(10);
    _field[randY][randX] = (rand == 0) ? 2 : 1;
}

/**
 * Moves block from position of block1 to position of block2
 * @param block1
 * @param block2
 */
void moveBlock(int *block1, int *block2) {
    *block2 = *block1;
    *block1 = 0;
}

/**
 * Calculates the power over the base
 * @param base
 * @param power
 * @return value of base raised to power(base ^ power)
 */
int power(int base, int power) {
    int out = 1;
    while (power > 0) {
        out *= base;
        power--;
    }
    return out;
}

/**
 * Joins block1 and block2
 * @param block1
 * @param block2
 */
void joinBlocks(int *block1, int *block2) {
    *block2 += 1;
    *block1 = 0;

    int block2Val = power(2, *block2);
    updateScore(block2Val);
}

/**
 * Checks to see if all values in the field are non-zero
 * @param _field
 * @return true(1) if field is full, false(0) otherwise
 */
int isFieldFull(Field _field) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (_field[i][j] == 0)
                return false;
        }
    }

    return true;
}