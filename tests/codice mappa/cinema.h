#ifndef CINEMA_H
#define CINEMA_H

#define ROWS 5
#define SEATS 9
#define DISABLED_ROW 2
#define DISABLED_SEATS 3
#define ROWS_BEFORE_CORRIDOR 5

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
    Seat seats[ROWS][SEATS];
} Hall;

// Funzioni dichiarate
void initialize_seats(Hall *hall);
char get_row_letter(int row_number);
void display_seats(Hall *hall);
void print_header();
void print_footer();
void print_header2();

#endif
