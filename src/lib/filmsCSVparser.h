#ifndef FILMSCSVPARSER_H
#define FILMSCSVPARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "cinema.h"


char* trim_whitespace(char* str);
void parse_films_csv_line(char *line, Film *film);
int calculate_total_seats(Film *film);
int count_showtimes(Film *film);
void read_films_csv(const char *filename, Film **films, int *film_count);
void free_films(Film *films, int film_count);
void print_films(char *buffer, size_t buffer_size, Film *films, int film_count);
void print_films_name(Film *films, int film_count);
int count_csv_lines(const char *filename);


#endif