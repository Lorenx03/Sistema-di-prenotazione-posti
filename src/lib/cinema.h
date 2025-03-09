#ifndef CINEMA_H
#define CINEMA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <pthread.h>

typedef enum {
    FREE,
    BOOKED,
    DISABLED,
    SELECTED
} SeatState;

// Struttura per il posto
typedef struct {
    char row;
    int seat_number;
    SeatState state;
    pthread_mutex_t lock;
    char booking_code[18];
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
void generateHallMapResponse(Hall *hall, char *buffer, size_t remaining_size);
// int bookSeat(Hall *hall, char *seat, char *bookingCode);
int bookSeats(Hall *hall, int numSeats, int seats[numSeats][2], char bookingCodes[numSeats][18]);
void printTicketToBuff(char **buff, char *bookingCode, char *filmTitle, char *filmShowtime, int row, int col, size_t *remaining_size);
// int unBookPrenotation(Hall *hall, char *prenotationCode);
int saveBookingsToFile(Films *filmsStruct, const char *filename);
int loadBookingsFromFile(Films *filmsStruct, const char *filename);
void freeFilmsList(Films *filmsStruct);

#endif