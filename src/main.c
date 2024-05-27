#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "lib/linkedList.h"
#include "lib/userInput.h"
#include "lib/gui.h"

int main() {
    TuiDisplayBuffer *displayBuff = createTuiDisplayBuffer();
    Style *style = createStyle(GREEN, BG_BLACK, 1, false, false, false);
	enableRawMode();

	char text[] = "Hello, World!";
	int rows, cols;
    getWindowSize(&rows, &cols);
    int len = strlen(text);
    int x = (cols - len) / 2;
    int y = rows / 2;

    drawText(displayBuff, x, y, text, style);

    //print display dimensions using drawText
    char dimensions[100];
    sprintf(dimensions, "Rows: %d, Cols: %d", rows, cols);
    drawText(displayBuff, 0, 0, dimensions, style);

	drawBuffer(displayBuff);

	signal(SIGINT, handle_sigint);
    while (1) {
        pause();
    }

    return 0;
}


