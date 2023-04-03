#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <memory.h>

/*  Local header files  */
#include "global.h"

/*  Arrow key char codes:   */
#define ARROW_DOWN 2
#define ARROW_UP 3
#define ARROW_LEFT 4
#define ARROW_RIGHT 5

#define LOGO_POS_X 6
#define WIN_WINDOW_ID 0
#define LOSS_WINDOW_ID 1

#define randFieldCoordinate() randInt(SIZE - 1)

WINDOW *fieldWindow;
WINDOW *scoreWindow;

int score = 0;
int maxBlock = 0;
int field[SIZE][SIZE] =
        {
                {1, 1, 2,  1},
                {2, 1, 2,  2},
                {1, 2, 1,  2},
                {2, 1, 2, 1}
        };

/**
 *
 * @param windowId
 */
void handlePopupWindow(int windowId);

/**
 * Determine what happens when the user presses a key of value <i>charCode</i>
 * @param charCode
 */
void handleInput(int charCode);

/**
 * Draws the field to the screen.
 * @param _field
 */
void drawField(int _field[SIZE][SIZE]);

/**
 * Returns a uniform-length character array representing a block on the field.
 *  All values are of length 7 -> 6 varying chars and string terminator.
 *
 *  Values are hardcoded to decrease execution time.
 * @param blockValue Holds block value.
 * @return A character array representing the block's value.
 */
char *displayBlock(int blockValue);

/**
 * Moves block from position of block1 to position of block2
 * @param block1
 * @param block2
 */
void moveBlock(int *block1, int *block2);

/**
 * Joins block1 and block2
 * @param block1
 * @param block2
 */
void joinBlocks(int *block1, int *block2);

/**
 * Moves and joins(where applicable) field blocks to the <i>left</i>.
 * @param _field Field we're iterating.
 */
int moveFieldLeft(int _field[SIZE][SIZE]);

/**
 * Moves and joins(where applicable) field blocks to the <i>right</i>.
 * @param _field Field we're iterating.
 */
int moveFieldRight(int _field[SIZE][SIZE]);

/**
 * Moves and joins(where applicable) field blocks <i>up</i>.
 * @param _field Field we're iterating.
 */
int moveFieldUp(int _field[SIZE][SIZE]);

/**
 * Moves and joins(where applicable) field blocks <i>down</i>.
 * @param _field Field we're iterating.
 */
int moveFieldDown(int _field[SIZE][SIZE]);

/**
 * Checks to see if all values in the field are non-zero
 * @param _field
 * @return
 */
int isFieldFull(int _field[SIZE][SIZE]);

/**
 * Safely converts 64bit long into 32bit int
 *  Note to self: INT32_MAX     01111111111111111111111111111111
 *                UINT32_MAX    11111111111111111111111111111111
 */
int longToInt(long l);

/**
 * Returns a random integer with a valute in the [0, upperLimit] range.
 * @param upperLimit Upper limit.
 * @return A random 32-bit integer.
 */
int randInt(int upperLimit);

/**
 * Populates a random block on the field.
 * @param _field
 */
void populateRandomBlock(int _field[SIZE][SIZE]);

/**
 * Calculates the power over the base
 * @param base
 * @param power
 * @return And integer value of base ^ power
 */
int power(int base, int power);

/**
 * Creates a new WINDOW with provided values.
 * @param height Height of the new window
 * @param width Width of the new window
 * @param posY Y coordinate of where the window will be created
 * @param posX X coordinate of where the window will be created
 * @return A reference to the newly created WINDOW
 */
WINDOW *createWindow(int height, int width, int posY, int posX, chtype color);

/**
 * Deletes <i>window</i> from screen and memory.
 * @param window
 */
void destroyWindow(WINDOW *window);

/**
 * Closes all the windows and stops execution of nc2048.
 */
void stop();

/**
 * 'Initializes' a new field.
 *  - Empties it
 *  - Spawns 2 blocks in random location
 * @param _field
 */
void initField(int _field[SIZE][SIZE]);

int main() {
    /*  Initialization  */
    srandom(time(NULL));
    // TODO: Remove comment -- initField(field);

    /*  Setting up ncurses. */
    initscr();              /*  Initializes the ncurses screen.               */
    raw();                  /*  Puts the terminal in raw mode.                */
    noecho();               /*  Don't echo input back to stdout.              */
    keypad(stdscr, TRUE);   /*  Enables keypad input(F-keys, arrows, ..)      */
    curs_set(0);            /*  Hiding the terminal cursor                    */

    if (has_colors()) {     /*  Checks to see if colors are supported by the terminal. */
        start_color();

        init_pair(1, COLOR_BLACK, COLOR_WHITE);
        init_pair(2, COLOR_CYAN, COLOR_BLACK);
        init_pair(3, COLOR_GREEN, COLOR_BLACK);
        init_pair(4, COLOR_RED, COLOR_BLACK);
    }

    refresh();

    /*   Setting up the field Window   */
    int fieldWindowHeight = SIZE + 2;
    int fieldWindowWidth = (SIZE * 6) + 5;
    int fieldWindowPosY = 7;
    int fieldWindowPosX = 2;
    fieldWindow = createWindow(
            fieldWindowHeight,
            fieldWindowWidth,
            fieldWindowPosY,
            fieldWindowPosX,
            COLOR_PAIR(2)
    );

    /*   Setting up the score Window   */
    int scoreWindowHeight = 7;
    int scoreWindowWidth = 16;
    int scoreWindowPosY = 7;
    int scoreWindowPosX = 36;
    scoreWindow = createWindow(
            scoreWindowHeight,
            scoreWindowWidth,
            scoreWindowPosY,
            scoreWindowPosX,
            COLOR_PAIR(2)
    );

    /* Printing nc2048 logo */
    mvprintw(0, LOGO_POS_X, "                mmmm   mmmm     mm   mmmm ");
    mvprintw(1, LOGO_POS_X, " m mm    mmm   \"   \"# m\"  \"m   m\"#  #    #");
    mvprintw(2, LOGO_POS_X, " #\"  #  #\"  \"      m\" #  m #  #\" #  \"mmmm\"");
    mvprintw(3, LOGO_POS_X, " #   #  #        m\"   #    # #mmm#m #   \"#");
    mvprintw(4, LOGO_POS_X, " #   #  \"#mm\"  m#mmmm  #mm#      #  \"#mmm\"");
    refresh();

    drawField(field);

    for (;;) {
        char in = getch();

        /*  Exit when q is pressed.*/
        if (in == 'q')
            break;

        handleInput((int) in);
    }

    stop();
    return 0;
}

/**
 *
 * @param out
 */
void drawDebug(char *out) {
#define START_Y (LINES-1)
#define START_X 0
    move(START_Y, START_X);
    clrtoeol();
    printw("%s", out);

    refresh();
#undef START_X
#undef START_Y
}

/**
 *  Draws the total score and max block value into the score window
 */
void drawScore() {
    werase(scoreWindow);
    wmove(scoreWindow, 1, 1);
    wprintw(scoreWindow, "Score:");

    wmove(scoreWindow, 2, 1);
    char ScoreStr[25];
    snprintf(ScoreStr, 25, "%d", score);
    wprintw(scoreWindow, " %s", ScoreStr);

    wmove(scoreWindow, 4, 1);
    wprintw(scoreWindow, "Highest Block:");

    wmove(scoreWindow, 5, 1);
    char MaxStr[25];
    snprintf(MaxStr, 25, "%d", maxBlock);
    wprintw(scoreWindow, " %s", MaxStr);
}

void drawField(int _field[SIZE][SIZE]) {
#define START_Y 1
#define START_X 1
    drawScore();

    wmove(fieldWindow, START_Y, START_X);

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            char *block = displayBlock(_field[i][j]);
            wprintw(fieldWindow, "%s", block);

            if (j != (SIZE - 1))
                wprintw(fieldWindow, " ");

            free(block);
        }

        wmove(fieldWindow, START_Y + (i + 1), START_X);
    }

    refresh();
    wrefresh(fieldWindow);
    wrefresh(scoreWindow);

#undef START_X
#undef START_Y
}

void handleInput(int charCode) {
    int moved = 0;
    switch (charCode) {
        case ARROW_DOWN:
            drawDebug("Pressed arrow DOWN.");
            moved = moveFieldDown(field);
            break;
        case ARROW_UP:
            drawDebug("Pressed arrow UP.");
            moved = moveFieldUp(field);
            break;
        case ARROW_LEFT:
            drawDebug("Pressed arrow LEFT.");
            moved = moveFieldLeft(field);
            break;
        case ARROW_RIGHT:
            drawDebug("Pressed arrow RIGHT.");
            moved = moveFieldRight(field);
            break;
        default:
            return;
    }

    if (moved > 0) {
        /* Handle winning condition - 'Did we make a block of value 2048?'*/
        if (maxBlock == 2048) {
            handlePopupWindow(WIN_WINDOW_ID);
            drawDebug("Congratulations! You won.");
            return;
        }

        populateRandomBlock(field);
        /* Handle losing condition - 'Are all the blocks on the field taken?'*/
        /*  TODO: Improve loss condition
         *      -> We only lose if we run out of possible move, a check is needed to ensure that is the case
         */
        if (isFieldFull(field) == TRUE) {
            handlePopupWindow(LOSS_WINDOW_ID);
            drawDebug("Oh no... You lost.");
            return;
        }

        /* Only redraw if there was a change on the field */
        drawField(field);
    }
}

int moveFieldLeft(int _field[SIZE][SIZE]) {
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

int moveFieldRight(int _field[SIZE][SIZE]) {
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

int moveFieldUp(int _field[SIZE][SIZE]) {
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

int moveFieldDown(int _field[SIZE][SIZE]) {
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

int longToInt(long l) {
    return UINT32_MAX & l;
}

int randInt(int upperLimit) {
    int divisor = RAND_MAX / (upperLimit + 1);
    int retval;

    do {
        retval = longToInt(random()) / divisor;
    } while (retval > upperLimit);

    return retval;
}

// TODO: Remove debug, make it more efficient if there's very little free spaces
void populateRandomBlock(int _field[SIZE][SIZE]) {
    int randY;
    int randX;

    do {
        randX = randFieldCoordinate();
        randY = randFieldCoordinate();
    } while (_field[randY][randX] != 0);

    int rand = randInt(10);
    _field[randY][randX] = (rand == 0) ? 2 : 1;
}

void moveBlock(int *block1, int *block2) {
    *block2 = *block1;
    *block1 = 0;
}

void joinBlocks(int *block1, int *block2) {
    *block2 += 1;
    *block1 = 0;

    int block2Val = power(2, *block2);
    score += block2Val;

    if (block2Val > maxBlock)
        maxBlock = block2Val;
}

int power(int base, int power) {
    int out = 1;
    while (power > 0) {
        out *= base;
        power--;
    }
    return out;
}

int isFieldFull(int _field[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (_field[i][j] == 0)
                return FALSE;
        }
    }

    return TRUE;
}

WINDOW *createWindow(int height, int width, int posY, int posX, chtype color) {
    WINDOW *window;
    window = newwin(height, width, posY, posX);
    wbkgdset(window, color);
    int err = box(window, 0, 0);
    if (err != OK) {
        drawDebug("Error");
    }
    wrefresh(window);
    return window;
}

void destroyWindow(WINDOW *window) {
    wborder(window, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    werase(window);
    wrefresh(window);
    delwin(window);
}

void stop() {
    /*  Manually exit ncurses window, prevents crashing the terminal.   */
    destroyWindow(fieldWindow);
    destroyWindow(scoreWindow);
    endwin();
    exit(0);
}

/**
 * Resets the game values and field.
 */
void reset() {
    initField(field);
    score = 0;
    maxBlock = 0;
}

/**
 * Sets all the values in the field to 0, effectively clearing it of all blocks.
 * @param _field
 */
void emptyField(int _field[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            _field[i][j] = 0;
}

void initField(int _field[SIZE][SIZE]) {
#define FIELD_SIZE (SIZE * SIZE * sizeof(int))
    emptyField(_field);

    populateRandomBlock(_field);
    populateRandomBlock(_field);
#undef FIELD_SIZE
}

/**
 * Create a new WINDOW, which is positioned in the approximate center of the terminal
 * @param height Height of the window
 * @param width Width of the window
 * @param color Color pair which will be used to draw the window with
 * @return
 */
WINDOW *createWindowCentered(int height, int width, int color) {
    int posY = (LINES / 2) - (height / 2);
    int posX = (COLS / 2) - (width / 2);

    WINDOW *centeredWindow = createWindow(height, width, posY, posX, color);

    return centeredWindow;
}

/**
 * Creates and populates a 'Winning' condition window
 * @return A reference to Winning WINDOW
 */
WINDOW *drawWin() {
#define WINDOW_HEIGHT 12
#define WINDOW_WIDTH 22
    WINDOW *winDow = createWindowCentered(WINDOW_HEIGHT, WINDOW_WIDTH, COLOR_PAIR(3));
    mvwprintw(winDow, 1, 3, "Congratulations!");
    mvwprintw(winDow, 3, 2, "You reached the ");
    mvwprintw(winDow, 4, 2, "maximum value of");
    mvwprintw(winDow, 5, 2, "2048. That's very");
    mvwprintw(winDow, 6, 2, "impressive.");
    mvwprintw(winDow, WINDOW_HEIGHT - 3, 3, "Press any key to");
    mvwprintw(winDow, WINDOW_HEIGHT - 2, 6, "continue..");
    wrefresh(winDow);
    return winDow;
#undef WINDOW_HEIGHT
#undef WINDOW_WIDTH
}

/**
 * Creates and populates a 'Losing' condition window
 * @return A reference to Losing WINDOW
 */
WINDOW *drawLoss() {
#define WINDOW_HEIGHT 12
#define WINDOW_WIDTH 22
    WINDOW *lossWindow = createWindowCentered(WINDOW_HEIGHT, WINDOW_WIDTH, COLOR_PAIR(4));
    mvwprintw(lossWindow, 1, 2, "Oh no... You lost!");
    mvwprintw(lossWindow, 3, 2, "Try a bit harder");
    mvwprintw(lossWindow, 4, 2, "on your next");
    mvwprintw(lossWindow, 5, 2, "attempt. I know");
    mvwprintw(lossWindow, 6, 2, "you can do it!");
    mvwprintw(lossWindow, WINDOW_HEIGHT - 3, 3, "Press any key to");
    mvwprintw(lossWindow, WINDOW_HEIGHT - 2, 6, "continue..");
    wrefresh(lossWindow);
    return lossWindow;
#undef WINDOW_HEIGHT
#undef WINDOW_WIDTH
}

/**
 * Refreshes all the main windows.
 *  -> std window, field window and score window
 */
void refreshScreen() {
    drawField(field);
    wrefresh(fieldWindow);
    wrefresh(scoreWindow);
    refresh();
}

void handlePopupWindow(int windowId) {
    WINDOW *window;
    refreshScreen();

    switch (windowId) {
        case WIN_WINDOW_ID:
            window = drawWin();
            break;
        case LOSS_WINDOW_ID:
            window = drawLoss();
            break;
        default:
            /* Invalid popup window id*/
            return;
    }
    refresh();

    /* Wait for input */
    char c = getch();

    /* Close pop-up window */
    destroyWindow(window);
    reset();
    refreshScreen();
}

char *displayBlock(int blockValue) {
    /*  Space for 6 chars + string terminator */
#define MAX_STR_LEN (7 * sizeof(char))
#define BLANK "[    ]"
    char *str = (char *) malloc(MAX_STR_LEN);

    if (blockValue == 0) {
        memcpy(str, BLANK, MAX_STR_LEN);
        return str;
    }
    if (blockValue == 1) {
        memcpy(str, "[   2]", MAX_STR_LEN);
        return str;
    }
    if (blockValue == 2) {
        memcpy(str, "[   4]", MAX_STR_LEN);
        return str;
    }
    if (blockValue == 3) {
        memcpy(str, "[   8]", MAX_STR_LEN);
        return str;
    }
    if (blockValue == 4) {
        memcpy(str, "[  16]", MAX_STR_LEN);
        return str;
    }
    if (blockValue == 5) {
        memcpy(str, "[  32]", MAX_STR_LEN);
        return str;
    }
    if (blockValue == 6) {
        memcpy(str, "[  64]", MAX_STR_LEN);
        return str;
    }
    if (blockValue == 7) {
        memcpy(str, "[ 128]", MAX_STR_LEN);
        return str;
    }
    if (blockValue == 8) {
        memcpy(str, "[ 256]", MAX_STR_LEN);
        return str;
    }
    if (blockValue == 9) {
        memcpy(str, "[ 512]", MAX_STR_LEN);
        return str;
    }
    if (blockValue == 10) {
        memcpy(str, "[1024]", MAX_STR_LEN);
        return str;
    }
    if (blockValue == 11) {
        memcpy(str, "[2048]", MAX_STR_LEN);
        return str;
    }

    memcpy(str, BLANK, MAX_STR_LEN);
    return str;
#undef MAX_STR_LEN
#undef BLANK
}
