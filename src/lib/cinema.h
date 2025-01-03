#ifndef CINEMA_H
#define CINEMA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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

#endif