#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>
#include "booking.h"

#define N 10

// Funzione per generare un codice di prenotazione univoco
int generate_booking_code() {
    return rand() % 90000 + 10000; // Genera un numero casuale tra 10000 e 99999
}

// Verifica se un posto è disponibile
int is_seat_available(Hall *hall, int row, int seat_num) {
    return !hall->seats[row][seat_num].is_booked && !hall->seats[row][seat_num].is_disabled;
}

// Prenotazione manuale di un posto
void book_seat(Hall *hall, int booking_code) {
    char row_input[N];
    char seat_input[N];
    char *endptr;

    while (1) {
        printf("Inserisci la fila (A-%c): ", get_row_letter(ROWS - 1));  // Corretto l'intervallo della fila
        if (fgets(row_input, sizeof(row_input), stdin) == NULL) {
            printf("Errore nella lettura della fila. Riprova.\n");
            continue; // Riprova se fgets fallisce
        }

        // Rimuove il carattere di nuova linea alla fine dell'input se presente
        row_input[strcspn(row_input, "\n")] = '\0';
        convert_to_uppercase(row_input);  // Converti in maiuscolo

        // Controlla se è stata inserita solo una lettera compresa tra A e l'ultima fila
        if (strlen(row_input) != 1 || !isalpha(row_input[0]) || row_input[0] < 'A' || row_input[0] >= get_row_letter(ROWS)) {
            printf("Fila non valida, riprova.\n");
            continue;
        }

        printf("Inserisci il numero del posto (1-%d): ", SEATS);
        if (fgets(seat_input, sizeof(seat_input), stdin) == NULL) {
            printf("Errore nella lettura del numero del posto. Riprova.\n");
            continue; // Riprova se fgets fallisce
        }

        int seat_num = strtol(seat_input, &endptr, 10);
        if (errno == ERANGE || *endptr != '\n' || seat_num < 1 || seat_num > SEATS) {
            printf("Numero del posto non valido, riprova.\n");
            continue;
        }

        int row = row_input[0] - 'A';  // Converti lettera a indice riga

        // Verifica disponibilità del posto
        if (!is_seat_available(hall, row, seat_num - 1)) {  // seat_num - 1 perché gli array iniziano da 0
            printf("Errore: il posto %c%d non è disponibile.\n", row_input[0], seat_num);
            continue;
        }

        // Prenota il posto
        hall->seats[row][seat_num - 1].is_booked = 1;
        hall->seats[row][seat_num - 1].booking_code = booking_code;

        printf("Prenotazione effettuata con successo per il posto %c%d! Codice di prenotazione: %d\n", row_input[0], seat_num, hall->seats[row][seat_num - 1].booking_code);
        break;  // Uscita dal ciclo una volta completata la prenotazione
    }
}


// Funzione per convertire una stringa in maiuscolo
void convert_to_uppercase(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] >= 'a' && str[i] <= 'z') {
            str[i] = str[i] - 'a' + 'A';
        }
    }
}

// Prenotazione automatica del miglior posto disponibile
void book_best_seat(Hall *hall, int num_tickets) {
    int booking_code = generate_booking_code();
    int seats_booked = 0;

    for (int i = 0; i < ROWS && seats_booked < num_tickets; i++) {
        for (int j = 0; j < SEATS && seats_booked < num_tickets; j++) {
            if (is_seat_available(hall, i, j)) {
                // Prenota il posto
                hall->seats[i][j].is_booked = 1;
                hall->seats[i][j].booking_code = booking_code;
                printf("Posto %c%d prenotato con il codice di prenotazione %d\n", get_row_letter(i), j + 1, booking_code);
                seats_booked++;
            }
        }
    }

    if (seats_booked == 0) {
        printf("Non ci sono abbastanza posti disponibili per la prenotazione.\n");
    } else {
        printf("Prenotazione completata per %d posti. Codice di prenotazione: %d\n", seats_booked, booking_code);
    }
}

// Funzione per cancellare la prenotazione usando il codice
void cancel_booking(Hall *hall, int booking_code) {
    int found = 0;

    // Cerca i posti con il codice di prenotazione
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < SEATS; j++) {
            if (hall->seats[i][j].booking_code == booking_code) {
                // Annulla la prenotazione
                hall->seats[i][j].is_booked = 0;
                hall->seats[i][j].booking_code = 0;
                found = 1;
                printf("Prenotazione per il posto %c%d è stata cancellata.\n", get_row_letter(i), j + 1);
            }
        }
    }

    if (!found) {
        printf("Errore: nessun posto trovato con il codice di prenotazione %d.\n", booking_code);
    } else {
        printf("Prenotazione con il codice %d cancellata correttamente.\n", booking_code);
    }
}
