#include "cinema.h"

int main() {
    // Stampa delle informazioni sui film
    Film films[] = {film1, film2, film3};

    // Inizializzazione delle sale
    initialize_seats(cinema1);
    initialize_seats(cinema2);
    initialize_seats(cinema3);

    srand(time(NULL)); // Inizializza il generatore di numeri casuali

    int choice;
    char buffer[N];
    char *endptr;

    while (1) {
        printf("\nCinema\n");
        printf("1. Programmazione\n");
        printf("2. Biglietteria\n");
        printf("3. Ordini e Prenotazioni\n");
        printf("4. Esci\n");

        printf("Inserisci la tua scelta: ");
        if (!fgets(buffer, N, stdin)) {
            continue;
        }
        choice = strtol(buffer, &endptr, 10);

        if (errno == ERANGE || endptr == buffer || (*endptr && *endptr != '\n') || (choice < 1 || choice > 4)) {
            printf("Scelta non valida, riprova.\n");
            continue;
        }

        switch (choice) {
            case 1:
                while (1) {
                    printf("\nProgrammazione:\n");

                    for (int i = 0; i < 3; i++) {
                        printf("Titolo: %s\n", films[i].titolo);
                        printf("Orari: %s\n", films[i].orari);
                        printf("\n");
                    }

                    printf("1. Visualizza dettagli di un film specifico\n\n");
                    printf("2. Torna al menu principale\n");
                    printf("Scegli un'opzione (1-2): ");

                    int sub_choice;
                    while (1) {
                        if (!fgets(buffer, N, stdin)) {
                            continue;
                        }
                        sub_choice = strtol(buffer, &endptr, 10);

                        if (errno == ERANGE || endptr == buffer || (*endptr && *endptr != '\n') || (sub_choice < 1 || sub_choice > 2)) {
                            printf("Opzione non valida, riprova.\n");
                            continue;
                        }
                        break;
                    }

                    if (sub_choice == 1) {
                        // Visualizza dettagli di un film specifico
                        printf("\nSeleziona il film di cui vuoi sapere i dettagli (1-3):\n1. Fast And Furious\n2. Titanic\n3. Piccolo Principe\n");
                        int film_choice;
                        while (1) {
                            if (!fgets(buffer, N, stdin)) {
                                continue;
                            }
                            film_choice = strtol(buffer, &endptr, 10);

                            if (errno == ERANGE || endptr == buffer || (*endptr && *endptr != '\n') || (film_choice < 1 || film_choice > 3)) {
                                printf("Scelta non valida, riprova.\n");
                                continue;
                            }
                            break;
                        }
                        printf("\nDettagli del film selezionato:\n");
                        printf("Titolo: %s\n", films[film_choice - 1].titolo);
                        printf("Genere: %s\n", films[film_choice - 1].genere);
                        printf("Lingua: %s\n", films[film_choice - 1].lingua);
                        printf("Durata: %d minuti\n", films[film_choice - 1].durata);
                        printf("Cast: %s\n", films[film_choice - 1].cast);
                        printf("Trama: %s\n", films[film_choice - 1].trama);
                        printf("Orari: %s\n", films[film_choice - 1].orari);
                        
                        printf("\n");
                    } else if (sub_choice == 2) {
                        // Torna al menu principale
                        break;
                    }
                }
                break;
            case 2:
                printf("\nBiglietteria\n");
                printf("\nLIMITI DI ACQUISTO: In fase di acquisto ogni cliente potrà acquistare fino ad un massimo di 4 biglietti.\nLa disponibilità dei posti è limitata.\n");
                                
                printf("\nProgrammazione:\n");

                
                for (int i = 0; i < 3; i++) {
                    printf("Sala %d - %s\n", i + 1, films[i].titolo);
                }

                printf("\nSeleziona il Film: ");
                
                int film_choice;
                while (1) {
                    if (!fgets(buffer, N, stdin)) {
                        continue;
                    }
                    film_choice = strtol(buffer, &endptr, 10);

                    if (errno == ERANGE || endptr == buffer || (*endptr && *endptr != '\n') || (film_choice < 1 || film_choice > 3)) {
                        printf("Scelta non valida, riprova.\n");
                        continue;
                    }
                    break;
                }
                printf("\nHai selezionato: %s\n", films[film_choice - 1].titolo);
                if (films[film_choice - 1].available_seats <= 0){
                    printf("\nFilm Sold-Out\n");
                    break;
                } else {
                    printf("\nBiglietti disponibili: %d\n\n", films[film_choice - 1].available_seats);
                }

                // Visualizzazione della sala e prenotazione dei posti
                switch (film_choice) {
                    case 1:
                        display_seats(cinema1);
                        break;
                    case 2:
                        display_seats(cinema2);
                        break;
                    case 3:
                        display_seats(cinema3);
                        break;
                }

                int num_tickets;
                while (1) {
                    printf("Quanti biglietti desideri acquistare? (Max 4): ");
                    if (!fgets(buffer, N, stdin)) {
                        continue;
                    }
                    num_tickets = strtol(buffer, &endptr, 10);

                    if (errno == ERANGE || endptr == buffer || (*endptr && *endptr != '\n') || (num_tickets < 1 || num_tickets > 4)) {
                        printf("Numero di biglietti non valido, riprova.\n");
                        continue;
                    }

                    // Gestione della selezione dei posti
                    switch (film_choice) {
                        case 1:
                            handle_seat_selection(cinema1, &films[film_choice - 1], num_tickets);
                            break;
                        case 2:
                            handle_seat_selection(cinema2, &films[film_choice - 1], num_tickets);
                            break;
                        case 3:
                            handle_seat_selection(cinema3, &films[film_choice - 1], num_tickets);
                            break;
                    }
                    break;
                }

                // Stampa i posti prenotati
                switch (film_choice) {
                    case 1:
                        print_booked_seats(cinema1);
                        break;
                    case 2:
                        print_booked_seats(cinema2);
                        break;
                    case 3:
                        print_booked_seats(cinema3);
                        break;
                }
                break;

            case 3: 
                printf("\nOrdini e Prenotazioni\n");
                printf("1. Visualizza Prenotazione\n");
                printf("2. Annulla Prenotazione\n");
                printf("3. Torna al menu principale\n");

                int sub_choice;
                while (1) {
                    printf("Scegli un'opzione (1-3): ");
                    if (!fgets(buffer, N, stdin)) {
                        continue;
                    }
                    sub_choice = strtol(buffer, &endptr, 10);

                    if (errno == ERANGE || endptr == buffer || (*endptr && *endptr != '\n') || (sub_choice < 1 || sub_choice > 3)) {
                        printf("Opzione non valida, riprova.\n");
                        continue;
                    }
                    break;
                }

                if (sub_choice == 1) {
                    // Visualizzazione di una prenotazione
                    printf("\nSeleziona la sala del film per visualizzare la prenotazione (1-3):\n1. Fast and Furious\n2. Titanic\n3. Il Piccolo Principe\n");
                    int film_choice;
                    while (1) {
                        if (!fgets(buffer, N, stdin)) {
                            continue;
                        }
                        film_choice = strtol(buffer, &endptr, 10);

                        if (errno == ERANGE || endptr == buffer || (*endptr && *endptr != '\n') || (film_choice < 1 || film_choice > 3)) {
                            printf("Scelta non valida, riprova.\n");
                            continue;
                        }
                        break;
                    }

                    switch (film_choice) {
                        case 1:
                            view_booking(cinema1, &films[film_choice - 1]);
                            break;
                        case 2:
                            view_booking(cinema2, &films[film_choice - 1]);
                            break;
                        case 3:
                            view_booking(cinema3, &films[film_choice - 1]);
                            break;
                    }
                } else if (sub_choice == 2) {
                    // Annullamento di una prenotazione
                    printf("\nSeleziona la sala del film per annullare la prenotazione (1-3):\n1. Fast and Furious\n2. Titanic\n3. Il Piccolo Principe\n");
                    int film_choice;
                    while (1) {
                        if (!fgets(buffer, N, stdin)) {
                            continue;
                        }
                        film_choice = strtol(buffer, &endptr, 10);

                        if (errno == ERANGE || endptr == buffer || (*endptr && *endptr != '\n') || (film_choice < 1 || film_choice > 3)) {
                            printf("Scelta non valida, riprova.\n");
                            continue;
                        }
                        break;
                    }

                    switch (film_choice) {
                        case 1:
                            cancel_booking(cinema1, &films[film_choice - 1]);
                            break;
                        case 2:
                            cancel_booking(cinema2, &films[film_choice - 1]);
                            break;
                        case 3:
                            cancel_booking(cinema3, &films[film_choice - 1]);
                            break;
                    }
                } else if (sub_choice == 3) {
                    // Torna al menu principale
                    break;
                }
                break;

            case 4:
                // Esci dal programma
                printf("Uscita dal programma...\n");
                exit(0);
                break;
        }
    }
    return 0;
}