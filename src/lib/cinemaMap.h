#ifndef CINEMA_MAP_H
#define CINEMA_MAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

// Print a string to the center of the terminal, with a specified width
void centerMapText(int columns, const char *format, ...);

// Appends the seats numbers to the buffer
void drawSeatNumbers(char **buffer, size_t *remaining_size, const int columns);

// Appends a separator line to the buffer
void drawSeparatorLine(char **buffer, size_t *remaining_size, const int columns);

// Generates the hall map GUI based on the hall map sent by the server. check cinema.h for the format.
void generateHallMap(char *map, char *buffer, size_t remaining_size, int rows, int columns);

#endif