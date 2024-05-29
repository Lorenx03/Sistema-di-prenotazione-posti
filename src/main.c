#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "lib/linkedList.h"
#include "lib/userInput.h"
#include "lib/gui.h"

void handle_sigint(int sig) {
    (void)sig;
    printf(RESET_COLOR "\n");
    disableRawMode();
    exit(0);
}

int main() {
    TuiDisplayBuffer *displayBuff = createTuiDisplayBuffer();
    Style *style = createStyle(GREEN, BG_GREEN, 1, false, false, false);
	enableRawMode();

	// char text[] = "Hello, World!";
	int rows, cols;
    getWindowSize(&rows, &cols);
    // int len = strlen(text);
    // int x = (cols - len) / 2;
    // int y = rows / 2;

    //drawText(displayBuff, x, y, text, style);

    //print display dimensions using drawText
    char dimensions[100];
    sprintf(dimensions, "Rows: %d, Cols: %d", rows, cols);
    drawBigText(displayBuff, 10, 10, "cinema", ' ', 1, 2, 1, style);
	drawBuffer(displayBuff);

	signal(SIGINT, handle_sigint);
    while (1) {
        pause();
    }

    return 0;
}