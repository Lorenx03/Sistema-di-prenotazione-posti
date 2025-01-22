#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <stdarg.h>

int safeStrToInt(char *buff);
void appendToBuffer(char **buffer_ptr, size_t *remaining_size, const char *format, ...);

#endif