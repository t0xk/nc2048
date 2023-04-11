#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "random.h"

/**
 * Seeds the random number generator
 */
void initRandom(){
    srandom(time(NULL));
}

/**
 * Safely converts 64bit long into 32bit int
 *  Note to self: INT32_MAX     01111111111111111111111111111111
 *                UINT32_MAX    11111111111111111111111111111111
 */
int longToInt(long l) {
    return UINT32_MAX & l;
}

/**
 * Returns a random integer with a value in the [0, upperLimit] range.
 * @param upperLimit Upper limit.
 * @return A random 32-bit integer.
 */
int randInt(int upperLimit) {
    int divisor = RAND_MAX / (upperLimit + 1);
    int retval;

    do {
        retval = longToInt(random()) / divisor;
    } while (retval > upperLimit);

    return retval;
}