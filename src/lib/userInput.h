#ifndef USERINPUT_H
#define USERINPUT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

// Waits for the user to press a key
void waitForKey();

// Reads an integer from stdin
void read_int(int *n);

// Reads a string from stdin
void read_str(char *str);

#endif