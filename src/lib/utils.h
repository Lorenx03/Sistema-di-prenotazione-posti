#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <stdarg.h>

int safeStrToInt(char *buff);
void getLine(char* str, int n, char* buffer, size_t buffer_size);
void appendToBuffer(char **buffer_ptr, size_t *remaining_size, const char *format, ...);
void generateRandomString(char *str, size_t length);
void getNthToken(char *str, char *delim, int n, char *token, size_t token_size);
void convertToUppercase(char *str);

#endif