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

void drawSeparatorLine(char **buffer, size_t *remaining_size, const int columns) {
    appendToBuffer(buffer, remaining_size, "  --");
    for (int j = 1; j <= columns; j++) {
        if (j<9)
            appendToBuffer(buffer, remaining_size, "-----");
        else
            appendToBuffer(buffer, remaining_size, "------");
    }
    appendToBuffer(buffer, remaining_size, "\n");
}

void generateHallMap(char *map, char *buffer, size_t remaining_size, int rows, int columns){ 
    // CINEMA
    drawSeatNumbers(&buffer, &remaining_size, columns);
    drawSeparatorLine(&buffer, &remaining_size, columns);

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

    // Legend
    appendToBuffer(&buffer, &remaining_size, "\nLegenda: \n\033[0;32m[A1]\033[0m Disponibile \n\033[0;31m[A1]\033[0m Prenotato \n\033[0;34m[A1]\033[0m Disabili\n");
}