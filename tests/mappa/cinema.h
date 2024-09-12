#ifndef CINEMA_H
#define CINEMA_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#define ROWS 8  // Dalla A alla G con un corridoio (fila vuota)
#define SEATS 9 // 9 posti per fila
#define DISABLED_SEATS 5
#define DISABLED_ROW 4 // Indice 4 corrisponde alla fila E
#define N 100
#define MAX_TICKETS 4

typedef struct {
    char row;
    int seat_number;
    int is_booked;
    int is_disabled;
    int booking_code; // Codice unico per la prenotazione
} Seat;

// Definizione della struttura Film
typedef struct {
    char titolo[50];
    char genere[30];
    char lingua[20];
    int durata; // Durata in minuti
    char cast[200];
    char trama[500];
    char orari[100];
    int available_seats; // Numero di posti disponibili
} Film;

// Funzioni di inizializzazione e gestione dei posti
void initialize_seats(Seat cinema[][SEATS]);
void display_seats(Seat cinema[][SEATS]);
char get_row_letter(int row_number);
void book_seat(Seat cinema[][SEATS], Film *film);
void print_header();
void print_header1();
void print_booked_seats(Seat cinema[][SEATS]);
void convert_to_uppercase(char *str);
void cancel_booking(Seat cinema[][SEATS], Film *film);
void view_booking(Seat cinema[][SEATS], Film *film);

#endif // CINEMA_H
