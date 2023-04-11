#include "score.h"

int score = 0;
int maxBlock = 0;

/**
 * Increments the current score counter. Also updates maxBlock value where applicable.
 * @param s Score increment
 */
void updateScore(int s) {
    score += s;

    if (s > maxBlock)
        maxBlock = s;
}