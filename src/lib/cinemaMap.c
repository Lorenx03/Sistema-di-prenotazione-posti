#include "cinemaMap.h"
#include "utils.h"
#include "cinema.h"

// ================================ HALL MAP ================================
void centerMapText(int columns, const char *format, ...) {
    char text[256];
    va_list args;
    va_start(args, format);
    vsnprintf(text, sizeof(text), format, args);
    va_end(args);

    int padding = (columns - strlen(text)) / 2;
    for (int i = 0; i < padding; i++) {
        printf(" ");
    }
    printf("%s", text);
}

void drawSeatNumbers(char **buffer, size_t *remaining_size, const int columns) {
    appendToBuffer(buffer, remaining_size, "    ");
    for (int j = 1; j <= columns; j++) {
        if (j<9)
            appendToBuffer(buffer, remaining_size, "%3d  ", j);
        else
            appendToBuffer(buffer, remaining_size, "%3d   ", j);
    }
    appendToBuffer(buffer, remaining_size, "\n");
}

int drawSeparatorLine(char **buffer, size_t *remaining_size, const int columns) {
    int width = 4;
    appendToBuffer(buffer, remaining_size, "  --");
    for (int j = 1; j <= columns; j++) {
        if (j<9){
            appendToBuffer(buffer, remaining_size, "-----");
            width += 5;
        }else{
            appendToBuffer(buffer, remaining_size, "------");
            width += 6;
        }
    }
    appendToBuffer(buffer, remaining_size, "\n");
    return width;
}

int generateHallMap(char *map, char *buffer, size_t remaining_size, int rows, int columns){
    if (map == NULL || buffer == NULL || rows <= 0 || columns <= 0) {
        fprintf(stderr, "generateHallMap: Invalid arguments\n");
        return -1;
    }

    // CINEMA
    drawSeatNumbers(&buffer, &remaining_size, columns);
    int width = drawSeparatorLine(&buffer, &remaining_size, columns);;

    // Seats
    for (int i = 0; i < rows; i++) {
        appendToBuffer(&buffer, &remaining_size, "%c | ", 'A'+i);

        for (int j = 0; j < columns; j++) {
            switch (map[i*columns + j] - '0') {
                case FREE:
                    appendToBuffer(&buffer, &remaining_size, "\033[0;32m[%c%d]\033[0m ", 'A'+i, j+1);
                    break;
                case BOOKED:
                    appendToBuffer(&buffer, &remaining_size, "\033[0;31m[%c%d]\033[0m ", 'A'+i, j+1);
                    break;
                case DISABLED:
                    appendToBuffer(&buffer, &remaining_size, "\033[0;34m[%c%d]\033[0m ", 'A'+i, j+1);
                    break;
                case SELECTED:
                    appendToBuffer(&buffer, &remaining_size, "\033[0;33m{%c%d}\033[0m ", 'A'+i, j+1);
                    break;

                default:
                    break;
            }
        }

        appendToBuffer(&buffer, &remaining_size, "| %c\n", 'A'+i);

        if(i == rows/3){
            appendToBuffer(&buffer, &remaining_size, "  |");
            for (int j = 1; j <= columns; j++) {
                if (j<9)
                    appendToBuffer(&buffer, &remaining_size, "     ");
                else
                    appendToBuffer(&buffer, &remaining_size, "      ");
            }
            appendToBuffer(&buffer, &remaining_size, "|\n");
        }
    }

    drawSeparatorLine(&buffer, &remaining_size, columns);
    drawSeatNumbers(&buffer, &remaining_size, columns);

    return width;
}