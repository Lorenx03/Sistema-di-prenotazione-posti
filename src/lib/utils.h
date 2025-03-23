#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <stdarg.h>
#include <unistd.h>

// Safe string to int conversion
int safeStrToInt(char *buff);

// gets the nth line from a string
void getLine(char* str, int n, char* buffer, size_t buffer_size);

// appends a formatted string to a buffer
void appendToBuffer(char **buffer_ptr, size_t *remaining_size, const char *format, ...);

// generates a random string of the given length
void generateRandomString(char *str, size_t length);

// gets the nth token from a string (uses strtok_r)
void getNthToken(char *str, char *delim, int n, char *token, size_t token_size);

// converts a string to uppercase
void convertToUppercase(char *str);

// deletes n bytes from a file
int fdeleteBytes(FILE* fp, int bytes);

#endif