#ifndef CINEMA_H
#define CINEMA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

// Struttura per il posto
typedef struct {
    char row;
    int seat_number;
    int is_booked;
    int is_disabled;
    int booking_code;
} Seat;

// Struttura per la sala
typedef struct {
    int rows;
    int columns;
    Seat **seats;
} Hall;

// Define the Film structure
typedef struct {
    char *name;
    char *genre;
    char *language;
    int duration;
    char *actors;
    char *plot;
    char *showtimes;
    int rows;      // Number of rows in the seating
    int columns;   // Number of columns in the seating
    int numbers_showtimes;  
    Hall *halls;
} Film;

// Define the Films structure
typedef struct {
    Film *list;
    int count;
} Films;

// Function to initialize the list of films
void initFilmsList(const char *filename, Films *filmsStruct);
void initialize_seats(Hall *hall, int rows, int columns);
void create_halls_for_showtimes(Film *film);
void generateHallMap(Hall *hall, char *buffer, size_t buffer_size);

#endif