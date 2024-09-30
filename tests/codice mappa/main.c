#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include "cinema.h"
#include "booking.h"

#define N 3

int main() {
    Hall hall;
    initialize_seats(&hall);
    srand(time(NULL)); // Inizializza il generatore di numeri casuali

    char buffer[N];
    char *endptr;
    int choice;
    int num_tickets;

    while (1) {
        printf("Vuoi prenotare o disdire un posto?\n");
        printf("1. Prenotare un posto\n");
        printf("2. Disdire una prenotazione\n");
        printf("3. Esci\n");
        printf("\nInserisci la tua scelta: ");
        
        // Leggi la scelta dell'utente
        if (!fgets(buffer, N, stdin)) {
            continue;
        }
        choice = strtol(buffer, &endptr, 10);

        // Controlla se l'input è valido
        if (errno == ERANGE || endptr == buffer || (*endptr && *endptr != '\n') || (choice < 1 || choice > 3)) {
            printf("Scelta non valida, riprova.\n");
            continue;
        }

        switch (choice) {
            case 1: // Prenotazione di un posto
                printf("Quanti posti vuoi prenotare (1-%d)? ", MAX_BOOKING_SEATS);
                if (!fgets(buffer, N, stdin)) {
                    printf("Errore nella lettura del numero di posti.\n");
                    continue;
                }
                num_tickets = atoi(buffer); // Converte l'input in un numero

                if (num_tickets < 1 || num_tickets > MAX_BOOKING_SEATS) {
                    printf("Errore: numero di posti non valido.\n");
                    continue;
                }

                // Scelta del tipo di prenotazione (automatica o manuale)
                while (1) {
                    printf("\nScegli l'opzione:\n");
                    printf("a) Miglior Posto (verrà assegnato il miglior posto disponibile)\n");
                    printf("b) Scelta manuale del posto\n");
                    printf("\nInserisci la tua scelta (a/b): ");

                    if (!fgets(buffer, N, stdin)) {
                        printf("Errore nella lettura dell'opzione.\n");
                        continue;
                    }

                    if ((buffer[0] == 'a' || buffer[0] == 'A') && buffer[1] == '\n') {
                        // Assegna automaticamente i posti migliori
                        book_best_seat(&hall, num_tickets);
                        break;
                    } else if ((buffer[0] == 'b' || buffer[0] == 'B') && buffer[1] == '\n') {
                        // Scelta manuale dei posti
                        int booking_code = generate_booking_code();
                        for (int i = 0; i < num_tickets; i++) {
                            printf("\nPosto numero %d:\n", i + 1);
                            display_seats(&hall); // Mostra i posti disponibili
                            book_seat(&hall, booking_code); // Chiedi all'utente di selezionare un posto
                        }
                        break;
                    } else {
                        printf("Scelta non valida, riprova.\n");
                    }
                }
                break;

            case 2: // Disdire una prenotazione
                {
                    char booking_code_input[10]; // Buffer per l'input
                    char *endptr; // Puntatore per strtol
                    int booking_code;

                    printf("Inserisci il codice di prenotazione da cancellare: ");
                    
                    if (!fgets(booking_code_input, sizeof(booking_code_input), stdin)) {
                        printf("Errore nella lettura del codice di prenotazione.\n");
                        continue;
                    }

                    // Rimuovi il newline finale se presente
                    if (booking_code_input[strlen(booking_code_input) - 1] == '\n') {
                        booking_code_input[strlen(booking_code_input) - 1] = '\0';
                    }

                    // Resetta errno prima della conversione
                    errno = 0;

                    // Usa strtol per convertire la stringa in intero e verifica eventuali errori
                    booking_code = strtol(booking_code_input, &endptr, 10);

                    if (errno == ERANGE || *endptr != '\0' || booking_code <= 0) {
                        printf("Errore: codice di prenotazione non valido. Riprova.\n");
                        continue;
                    }

                    // Cancella la prenotazione
                    cancel_booking(&hall, booking_code); 
                }
                break;


            case 3: // Uscita dal programma
                printf("Uscita dal programma.\n");
                return 0;

            default:
                printf("Scelta non valida, riprova.\n");
        }

        // Visualizza la sala aggiornata dopo ogni operazione
        display_seats(&hall);
    }

    return 0;
}
