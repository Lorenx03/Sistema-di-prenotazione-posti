#ifndef FILMSCSVPARSER_H
#define FILMSCSVPARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "cinema.h"

// Function to remove leading and trailing whitespace
char* trimWhitespace(char* str);

// Function to parse a CSV line, handling quoted fields with commas
void parseFilmsCsvLine(char *line, Film *film);

// Function to count the number of seats
int calculateTotalSeats(Film *film);

// Function to count the number of showtimes
int countShowtimes(Film *film);

// Function to read the CSV file and store the data into the Film array
void readFilmsCsv(const char *filename, Film **films, int *film_count);

// Function to free the memory allocated for the films
void freeFilms(Film *films, int film_count);

// Function to print film details to a buffer
void printFilms(char *buffer, size_t buffer_size, Film *films, int film_count);

// Function to print the names of the films to a buffer
void printFilmsName(char *buffer, size_t buffer_size, Film *films, int film_count);

// Function to count the number of lines in the CSV file
int countCsvLines(const char *filename);

#endif