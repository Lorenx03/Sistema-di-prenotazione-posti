#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <string.h>
#include <signal.h>
#include <math.h>

#include "alphabet.h"
#define RESET_COLOR "\x1b[0m"

// Define ANSI escape codes for basic colors
const char *colors[] = {
    "\x1b[30m", // Black
    "\x1b[31m", // Red
    "\x1b[32m", // Green
    "\x1b[33m", // Yellow
    "\x1b[34m", // Blue
    "\x1b[35m", // Magenta
    "\x1b[36m", // Cyan
    "\x1b[37m"  // White
};

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

// Define ANSI escape codes for basic background colors
const char *bg_colors[] = {
    "\x1b[40m", // Black background
    "\x1b[41m", // Red background
    "\x1b[42m", // Green background
    "\x1b[43m", // Yellow background
    "\x1b[44m", // Blue background
    "\x1b[45m", // Magenta background
    "\x1b[46m", // Cyan background
    "\x1b[47m"  // White background
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


// Define ANSI escape codes for text styles
typedef struct style{
    enum textColors textColor;
    enum bgColors backgroundColor;
    bool isBold;
    bool isItalic;
    bool isUnderlined;
    bool isInverse;
} Style;

Style defaultStyle = {GREEN, BG_BLACK, true, false, false, false};


typedef struct cell{
    Style *style;
    char character;
} Cell;

typedef struct tuiDisplayBuffer{
    int height;
    int width;
    Cell **buffer;
} TuiDisplayBuffer;





/* ------------------------ Setup Terminal ------------------------ */

struct termios orig_termios;

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    printf("\x1b[?1049l"); // Restore to normal buffer
}

void enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_iflag &= ~(IXON);
    raw.c_oflag &= ~(OPOST);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    printf("\x1b[?1049h"); // Use alternate buffer
}

void getWindowSize(int *rows, int *cols) {
    struct winsize ws;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    *rows = ws.ws_row;
    *cols = ws.ws_col;
}




/* ----------------------- Display buffer -----------------------*/

Style *createStyle(enum textColors textColor, enum bgColors backgroundColor, bool isBold, bool isItalic, bool isUnderlined, bool isInverse){
    Style *style = malloc(sizeof(Style));
    if (style == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    style->textColor = textColor;
    style->backgroundColor = backgroundColor;
    style->isBold = isBold;
    style->isItalic = isItalic;
    style->isUnderlined = isUnderlined;
    style->isInverse = isInverse;

    return style;
}


TuiDisplayBuffer *createTuiDisplayBuffer(){
    TuiDisplayBuffer *displayBuff = malloc(sizeof(TuiDisplayBuffer));
    if (displayBuff == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    getWindowSize(&(displayBuff->height), &(displayBuff->width));

    displayBuff->buffer = malloc(displayBuff->height * sizeof(Cell*));
    if (displayBuff->buffer == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < displayBuff->height; i++) {
        displayBuff->buffer[i] = malloc(displayBuff->width * sizeof(Cell));

        if (displayBuff->buffer[i] == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
    }

    // Initialize buffer with spaces
    for (int y = 0; y < displayBuff->height; y++){
        for (int x = 0; x < displayBuff->width; x++){
            displayBuff->buffer[y][x].style = &defaultStyle;
            displayBuff->buffer[y][x].character = ' ';
        }
    }

    return displayBuff;
}


/* ------------------------ Draw ------------------------ */

void drawText(TuiDisplayBuffer *displayBuff, int x, int y, const char *text, Style *style){
    int len = strlen(text);

    if(x < 0 || y < 0 || x + len > displayBuff->width || y > displayBuff->height){
        fprintf(stderr, "Invalid coordinates\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < len; i++){
        displayBuff->buffer[y][x + i].character = text[i];
        displayBuff->buffer[y][x + i].style = style;
    }
}




void drawFilledRettangle(TuiDisplayBuffer *displayBuff, int start_x, int start_y, int end_x, int end_y, char ch, Style *style){
    if(start_x < 0 || start_y < 0 || end_x > displayBuff->width || end_y > displayBuff->height){
        fprintf(stderr, "Invalid coordinates\n");
        exit(EXIT_FAILURE);
    }

    if(start_x >= end_x){
        int temp = start_x;
        start_x = end_x;
        end_x = temp;
    }

    if(start_y >= end_y){
        int temp = start_y;
        start_y = end_y;
        end_y = temp;
    }

    for (int i = start_y; i < end_y; i++){
        for (int j = start_x; j < end_x; j++){
            displayBuff->buffer[i][j].character = ch;
            displayBuff->buffer[i][j].style = style;
        }
    }
}



void drawHollowRettangle(TuiDisplayBuffer *displayBuff, int start_x, int start_y, int end_x, int end_y, char ch1 , char ch2, Style *style1, Style *style2){
    if(start_x < 0 || start_y < 0 || end_x > displayBuff->width || end_y > displayBuff->height){
        fprintf(stderr, "Invalid coordinates\n");
        exit(EXIT_FAILURE);
    }

    if(start_x >= end_x){
        int temp = start_x;
        start_x = end_x;
        end_x = temp;
    }

    if(start_y >= end_y){
        int temp = start_y;
        start_y = end_y;
        end_y = temp;
    }

    for (int i = start_x; i < end_x; i++){
        displayBuff->buffer[start_y][i].character = ch1;
        displayBuff->buffer[end_y - 1][i].character = ch1;

        displayBuff->buffer[start_y][i].style = style1;
        displayBuff->buffer[end_y - 1][i].style = style1;
    }

    for (int i = start_y; i < end_y; i++){
        displayBuff->buffer[i][start_x].character = ch2;
        displayBuff->buffer[i][end_x - 1].character = ch2;

        displayBuff->buffer[i][start_x].style = style2;
        displayBuff->buffer[i][end_x - 1].style = style2;
    }
}



void drawBuffer(TuiDisplayBuffer *displayBuff){
    printf("\x1b[2J"); // Clear screen
    for (int y = 0; y < displayBuff->height; y++){
        for (int x = 0; x < displayBuff->width; x++){
            Cell cell = displayBuff->buffer[y][x];
            printf("%s%s", colors[cell.style->textColor], bg_colors[cell.style->backgroundColor]);
            if (cell.style->isBold) {
                printf("\x1b[1m");
            }
            if (cell.style->isItalic) {
                printf("\x1b[3m");
            }
            if (cell.style->isUnderlined) {
                printf("\x1b[4m");
            }
            if (cell.style->isInverse) {
                printf("\x1b[7m");
            }
            printf("%c%s", cell.character, RESET_COLOR);
        }
    }
    fflush(stdout);
}





void drawFilledCircle(TuiDisplayBuffer *displayBuff, int x, int y, int radius, char ch, Style *style){
    if(x < 0 || y < 0 || x + radius > displayBuff->width || y + radius > displayBuff->height){
        fprintf(stderr, "Invalid coordinates\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < radius; i++){
        for (int j = 0; j < radius; j++){
            if(i * i + j * j <= radius * radius){
                displayBuff->buffer[y + i][x + j].character = ch;
                displayBuff->buffer[y + i][x - j].character = ch;
                displayBuff->buffer[y - i][x + j].character = ch;
                displayBuff->buffer[y - i][x - j].character = ch;

                displayBuff->buffer[y + i][x + j].style = style;
                displayBuff->buffer[y + i][x - j].style = style;
                displayBuff->buffer[y - i][x + j].style = style;
                displayBuff->buffer[y - i][x - j].style = style;
            }
        }
    }
}



void drawLine(TuiDisplayBuffer *displayBuff, int x1, int y1, int x2, int y2, char ch, Style *style){
    if(x1 < 0 || y1 < 0 || x2 > displayBuff->width || y2 > displayBuff->height){
        fprintf(stderr, "Invalid coordinates\n");
        exit(EXIT_FAILURE);
    }

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;

    while (x1 != x2 || y1 != y2){
        displayBuff->buffer[y1][x1].character = ch;
        displayBuff->buffer[y1][x1].style = style;

        int e2 = 2 * err;
        if (e2 > -dy){
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx){
            err += dx;
            y1 += sy;
        }
    }
}


enum alphabetChars letterToIndex(char letter){
    if (letter >= 'a' && letter <= 'z'){
        return letter - 'a';
    } else if (letter >= 'A' && letter <= 'Z'){
        return letter - 'A';
    } else {
        return -1;
    }
}


void drawBigText(TuiDisplayBuffer *displayBuff, int x, int y, const char *text, char ch, short spacing, int xMultiplier, int yMultiplier, Style *style){
    int len = strlen(text);
    int totalWidth = 0;
    int totalHeight = 5 * yMultiplier;

    if(xMultiplier < 1 || yMultiplier < 1){
        fprintf(stderr, "Invalid multipliers\n");
        exit(EXIT_FAILURE);
    }

    int index = 0;
    for (int i = 0; i < len; i++) {
        index = letterToIndex(text[i]);
        if (index == -1){
            fprintf(stderr, "Invalid character\n");
            exit(EXIT_FAILURE);
        }

        totalWidth += alphabet[index].width * xMultiplier + spacing;
    }

    if (x < 0 || y < 0 || x + totalWidth > displayBuff->width || y + totalHeight > displayBuff->height){
        fprintf(stderr, "Invalid coordinates\n");
        exit(EXIT_FAILURE);
    }

    index = 0;
    for (int i = 0; i < len; i++){
        index = letterToIndex(text[i]);
        for (int j = 0; j < 5; j++){
            for (int k = 0; k < alphabet[index].width; k++){
                if (alphabet[index].font[j][k] == '#'){
                    for (int m = 0; m < xMultiplier; m++){
                        for (int n = 0; n < yMultiplier; n++){
                            displayBuff->buffer[y + j * yMultiplier + n][x + k * xMultiplier + m].character = ch;
                            displayBuff->buffer[y + j * yMultiplier + n][x + k * xMultiplier + m].style = style;
                        }
                    }
                }
            }
        }
        x += (alphabet[index].width * xMultiplier) + spacing;
    }
}

/* ------------------------ Main ------------------------ */

// int main() {
//     enableRawMode();

//     char message[100];
//     drawCenteredText(message);

//     signal(SIGINT, handle_sigint);

//     char ch;
//     while (read(STDIN_FILENO, &ch, 1) > 0) {
//         if (ch == '\x1b') { // Check for escape character
//             // Read the next two characters to determine the arrow key
//             char seq[2];
//             if (read(STDIN_FILENO, &seq[0], 1) != 1 || read(STDIN_FILENO, &seq[1], 1) != 1) {
//                 break;
//             }

//             if (seq[0] == '[') {
//                 switch (seq[1]) {
//                     case 'A': // Up arrow key
//                         snprintf(message, 100, "You pressed the up arrow key");
//                         drawCenteredText(message);
//                         break;
//                     case 'B': // Down arrow key
//                         snprintf(message, 100, "You pressed the down arrow key");
//                         drawCenteredText(message);
//                         break;
//                     case 'C': // Right arrow key
//                         snprintf(message, 100, "You pressed the right arrow key");
//                         drawCenteredText(message);
//                         break;
//                     case 'D': // Left arrow key
//                         snprintf(message, 100, "You pressed the left arrow key");
//                         drawCenteredText(message);
//                         break;
//                 }
//             }
//         } else if (ch == 'q') {
//             break;
//         }
//     }

//     return 0;
// }