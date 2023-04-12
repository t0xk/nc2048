#include <stdlib.h>
#include <ncurses.h>
#include <memory.h>

/*  Local header files  */
#include "global.h"
#include "score.h"
#include "field.h"
#include "random.h"

/*  Arrow key char codes:   */
#define ARROW_DOWN 2
#define ARROW_UP 3
#define ARROW_LEFT 4
#define ARROW_RIGHT 5

#define LOGO_POS_X 6
#define WIN_WINDOW_ID 0
#define LOSS_WINDOW_ID 1

WINDOW *fieldWindow;
WINDOW *scoreWindow;
Field field;

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
void drawField(Field _field);

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

int main() {
    /*  Initialization  */
    initRandom();
    initField(field);

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
    mvprintw(0, LOGO_POS_X, "               mmmm   mmmm     mm   mmmm ");
    mvprintw(1, LOGO_POS_X, "m mm    mmm   \"   \"# m\"  \"m   m\"#  #    #");
    mvprintw(2, LOGO_POS_X, "#\"  #  #\"  \"      m\" #  m #  #\" #  \"mmmm\"");
    mvprintw(3, LOGO_POS_X, "#   #  #        m\"   #    # #mmm#m #   \"#");
    mvprintw(4, LOGO_POS_X, "#   #  \"#mm\"  m#mmmm  #mm#      #  \"#mmm\"");
    mvprintw(5, LOGO_POS_X, "            Press 'Q' to exit.");
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
 * Draws a debug string in the bottom-left corner of the terminal.
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
    box(scoreWindow, 0, 0);
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

void drawField(Field _field) {
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
        if (isFieldFull(field) == true) {
            if (isFieldMovable(field) == false) {
                handlePopupWindow(LOSS_WINDOW_ID);
                drawDebug("Oh no... You lost.");
                return;
            }
        }

        /* Only redraw if there was a change on the field */
        drawField(field);
    }
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
    box(fieldWindow, 0, 0);
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
