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
    char **buffer;
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

TuiDisplayBuffer *createTuiDisplayBuffer(){
    TuiDisplayBuffer *displayBuff = malloc(sizeof(TuiDisplayBuffer));
    if (displayBuff == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    getWindowSize(&(displayBuff->height), &(displayBuff->width));

    displayBuff->buffer = malloc(displayBuff->height * sizeof(char *));
    if (displayBuff->buffer == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < displayBuff->height; i++) {
        displayBuff->buffer[i] = malloc(displayBuff->width * sizeof(char));

        if (displayBuff->buffer[i] == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
    }

    // Initialize buffer with spaces
    for (int y = 0; y < displayBuff->height; y++){
        for (int x = 0; x < displayBuff->width; x++){
            displayBuff->buffer[y][x] = ' ';
        }
    }

    return displayBuff;
}


/* ------------------------ Draw ------------------------ */

void drawText(TuiDisplayBuffer *displayBuff, int x, int y, const char *text){
    int len = strlen(text);

    if(x < 0 || y < 0 || x + len > displayBuff->width || y > displayBuff->height){
        fprintf(stderr, "Invalid coordinates\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < len; i++){
        displayBuff->buffer[y][x + i] = text[i];
    }
}




void drawFilledRettangle(TuiDisplayBuffer *displayBuff, int start_x, int start_y, int end_x, int end_y, char ch){
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
            displayBuff->buffer[i][j] = ch;
        }
    }
}


void drawHollowRettangle(TuiDisplayBuffer *displayBuff, int start_x, int start_y, int end_x, int end_y, char ch1 , char ch2){
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
        displayBuff->buffer[start_y][i] = ch1;
        displayBuff->buffer[end_y - 1][i] = ch1;
    }

    for (int i = start_y; i < end_y; i++){
        displayBuff->buffer[i][start_x] = ch2;
        displayBuff->buffer[i][end_x - 1] = ch2;
    }
}



void drawBuffer(TuiDisplayBuffer *displayBuff){
    printf("\x1b[2J"); // Clear screen
    for (int y = 0; y < displayBuff->height; y++){
        for (int x = 0; x < displayBuff->width; x++){
            printf("\x1b[%d;%dH%c", y, x, displayBuff->buffer[y][x]); // Move cursor and print text
        }
    }
    fflush(stdout);
}





void handle_sigint(void) {
    disableRawMode();
    exit(0);
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