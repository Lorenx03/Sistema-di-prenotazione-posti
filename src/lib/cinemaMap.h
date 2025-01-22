#ifndef CINEMA_MAP_H
#define CINEMA_MAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

void drawSeatNumbers(char **buffer, size_t *remaining_size, const int columns);
void drawSeparatorLine(char **buffer, size_t *remaining_size, const int columns);
void generateHallMap(char *map, char *buffer, size_t remaining_size, int rows, int columns);

#endif