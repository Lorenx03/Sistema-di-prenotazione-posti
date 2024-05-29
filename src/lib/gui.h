#ifndef GUI_H
#define GUI_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <string.h>
#include <signal.h>
#include <math.h>


enum textColors {
    BLACK,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE
};

enum bgColors {
    BG_BLACK,
    BG_RED,
    BG_GREEN,
    BG_YELLOW,
    BG_BLUE,
    BG_MAGENTA,
    BG_CYAN,
    BG_WHITE
};


typedef struct style{
    enum textColors textColor;
    enum bgColors backgroundColor;
    bool isBold;
    bool isItalic;
    bool isUnderlined;
    bool isInverse;
} Style;


typedef struct cell{
    char textColor[7];
    char backgroundColor[7];
    char character;
} Cell;

typedef struct tuiDisplayBuffer{
    int height;
    int width;
    Cell **buffer;
} TuiDisplayBuffer;



/* ------------------------ Setup Terminal ------------------------ */
struct termios orig_termios;

void disableRawMode();
void enableRawMode();
void getWindowSize(int *rows, int *cols);

Style *createStyle(enum textColors textColor, enum bgColors backgroundColor, bool isBold, bool isItalic, bool isUnderlined, bool isInverse);
TuiDisplayBuffer *createTuiDisplayBuffer();


/* ------------------------ Draw ------------------------ */

void drawText(TuiDisplayBuffer *displayBuff, int x, int y, const char *text, Style *style);
void drawFilledRettangle(TuiDisplayBuffer *displayBuff, int start_x, int start_y, int end_x, int end_y, char ch, Style *style);
void drawHollowRettangle(TuiDisplayBuffer *displayBuff, int start_x, int start_y, int end_x, int end_y, char ch1 , char ch2, Style *style1, Style *style2);
void drawFilledCircle(TuiDisplayBuffer *displayBuff, int x, int y, int radius, char ch, Style *style);
//void drawHollowCircle(TuiDisplayBuffer *displayBuff, int x, int y, int radius, char ch, Style *style);
void drawBigText(TuiDisplayBuffer *displayBuff, int x, int y, const char *text, char ch, short spacing, int xMultiplier, int yMultiplier, Style *style);
void drawLine(TuiDisplayBuffer *displayBuff, int x1, int y1, int x2, int y2, char ch, Style *style);
void drawBuffer(TuiDisplayBuffer *displayBuff);
void handle_sigint(int sig);

#endif