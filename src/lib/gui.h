#ifndef GUI_H
#define GUI_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <string.h>
#include <signal.h>


typedef struct tuiDisplayBuffer{
    int height;
    int width;
    wchar_t **buffer;
} TuiDisplayBuffer;


/* ------------------------ Setup Terminal ------------------------ */
struct termios orig_termios;

void disableRawMode();
void enableRawMode();
void getWindowSize(int *rows, int *cols);
TuiDisplayBuffer *createTuiDisplayBuffer();


/* ------------------------ Draw ------------------------ */

void drawText(TuiDisplayBuffer *displayBuff, int x, int y, const char *text);
void drawFilledRettangle(TuiDisplayBuffer *displayBuff, int start_x, int start_y, int end_x, int end_y, char ch);
void drawHollowRettangle(TuiDisplayBuffer *displayBuff, int start_x, int start_y, int end_x, int end_y, char ch1, char ch2);
void drawBuffer(TuiDisplayBuffer *displayBuff);
void handle_sigint(int sig);

#endif