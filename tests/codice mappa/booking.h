#ifndef BOOKING_H
#define BOOKING_H
#define MAX_BOOKING_SEATS 4

#include "cinema.h"


// Funzioni per la prenotazione
void book_seat(Hall *hall, int booking_code);
void book_best_seat(Hall *hall, int num_tickets);
int generate_booking_code();
int is_seat_available(Hall *hall, int row, int seat_num);
void cancel_booking(Hall *hall, int booking_code);
void convert_to_uppercase(char *str);

#endif
