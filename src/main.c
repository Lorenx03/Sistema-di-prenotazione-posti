#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib/linkedList.h"
#include "lib/userInput.h"
#include "lib/gui.h"

int main() {
    TuiDisplayBuffer *displayBuff = createTuiDisplayBuffer();
	enableRawMode();

	char text[] = "Hello, World!";
	int rows, cols;
    getWindowSize(&rows, &cols);
    int len = strlen(text);
    int x = (cols - len) / 2;
    int y = rows / 2;

	drawText(displayBuff, x, y, text);

	drawFilledRettangle(displayBuff, 10, 10, 20, 20, '#');
	drawFilledRettangle(displayBuff, 20, 20, 40, 40, '#');

	drawBuffer(displayBuff);

	signal(SIGINT, handle_sigint);
    while (1) {
        pause();
    }

    return 0;
}


