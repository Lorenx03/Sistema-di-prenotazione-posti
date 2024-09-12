#include "cinema.h"

// Funzione per inizializzare i posti
void initialize_seats(Seat cinema[][SEATS]) {
    for (int i = 0; i < ROWS; i++) {
        // Saltare la fila vuota tra D ed E
        if (i == 4) {
            continue;
        }
        for (int j = 0; j < SEATS; j++) {
            cinema[i][j].row = get_row_letter(i);
            cinema[i][j].seat_number = j + 1;
            cinema[i][j].is_booked = 0;
            cinema[i][j].is_disabled = 0;
            cinema[i][j].booking_code = 0;
        }
    }

    // Riservare 5 poltrone per disabili nella fila E
    for (int j = 0; j < DISABLED_SEATS; j++) {
        cinema[DISABLED_ROW][j].is_disabled = 1;
    }
}

// Funzione per ottenere la lettera della riga
char get_row_letter(int row_number) {
    if (row_number >= 5) {
        return 'A' + row_number - 1;
    } else {
        return 'A' + row_number;
    }
}

// Funzione per visualizzare i posti
void display_seats(Seat cinema[][SEATS]) {
    print_header();
    for (int i = 0; i < ROWS; i++) {
        if (i == 4) {
            printf("\n");
            continue;
        }
        printf("%c | ", cinema[i][0].row);
        for (int j = 0; j < SEATS; j++) {
            if (cinema[i][j].is_disabled) {
                printf("\033[0;34m[D]\033[0m "); // Blu per disabili
            } else if (cinema[i][j].is_booked) {
                printf("\033[0;31m[X]\033[0m "); // Rosso per prenotato
            } else {
                printf("\033[0;32m[O]\033[0m "); // Verde per disponibile
            }
        }
        printf("| %c\n", cinema[i][0].row);
    }
    print_header1();
    printf("\nLegenda: \033[0;32m[O]\033[0m Disponibile, \033[0;31m[X]\033[0m Prenotato, \033[0;34m[D]\033[0m Riservato per disabili\n");
}

// Funzione per stampare l'intestazione della visualizzazione dei posti
void print_header() {
    printf("   ");
    for (int j = 1; j <= SEATS; j++) {
        printf("%3d ", j);
    }
    printf("\n   +");
    for (int j = 1; j <= SEATS; j++) {
        printf("---+");
    }
    printf("\n");
}

// Funzione per stampare l'intestazione della visualizzazione dei posti
void print_header1() {
    printf("   ");
    for (int j = 1; j <= SEATS; j++) {
        printf("+---");
    }
    printf("+\n   ");
    for (int j = 1; j <= SEATS; j++) {
        printf("%3d ", j);
    }
}

// Funzione per prenotare un posto
void book_seat(Seat cinema[][SEATS], Film *film) {
    char row[N];
    char seat[N];
    int sceltaPosto;
    int sceltaFila;
    int booking_code = rand() % 10000 + 1; // Genera un codice di prenotazione unico
    char *endptr;

    while (1) {
        printf("Inserisci la fila (A-G): ");
        if (!fgets(row, N, stdin)) {
            return;
        }

        convert_to_uppercase(row);

        // Controlla se è stata inserita solo una lettera compresa tra A e G
        if (strlen(row) != 2 || !isalpha(row[0]) || row[0] < 'A' || row[0] > 'G' || row[1] != '\n') {
            printf("Fila non valida, riprova.\n");
            continue;
        }

        printf("Inserisci il numero del posto (1-9): ");
        if (!fgets(seat, N, stdin)) {
            return;
        }

        // Converte l'input in un intero
        sceltaPosto = strtol(seat, &endptr, 10);

        // Controlla se l'input è un numero compreso tra 1 e 9
        if (errno == ERANGE || endptr == seat || (*endptr && *endptr != '\n') || sceltaPosto < 1 || sceltaPosto > 9) {
            printf("Numero del posto non valido, riprova.\n");
            continue;
        }

        int row_index = row[0] - 'A';
        int seat_index = strtol(seat, NULL, 10) - 1;

        // Correggere l'indice di riga per saltare la fila vuota
        if (row_index >= 4) {
            row_index += 1;
        }

        // Controllare se la fila è valida
        if (row_index < 0 || row_index >= ROWS || seat_index < 0 || seat_index >= SEATS) {
            printf("Fila o numero del posto non valido.\n");
            return;
        }

        if (cinema[row_index][seat_index].is_booked) {
            printf("Il posto è già prenotato.\n");
            continue;
        } else {
            cinema[row_index][seat_index].is_booked = 1;
            cinema[row_index][seat_index].booking_code = booking_code; // Assegna il codice di prenotazione
            film->available_seats--; // Decrementa il numero di posti disponibili
            printf("Prenotazione effettuata con successo. Codice di prenotazione: %d\n", booking_code);
        }

        break; // Esce dal ciclo se l'input è valido
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

// Funzione per annullare una prenotazione
void cancel_booking(Seat cinema[][SEATS], Film *film) {
    char row[N];
    char seat[N];
    int booking_code;
    char *endptr;

    printf("Inserisci il codice di prenotazione per annullare: ");
    if (!fgets(row, N, stdin)) {
        return;
    }
    booking_code = strtol(row, &endptr, 10);

    // Verifica se il codice di prenotazione è valido
    if (errno == ERANGE || endptr == row || (*endptr && *endptr != '\n')) {
        printf("Codice di prenotazione non valido, riprova.\n");
        return;
    }

    int found = 0;
    for (int i = 0; i < ROWS; i++) {
        if (i == 4) {
            continue;
        }
        for (int j = 0; j < SEATS; j++) {
            if (cinema[i][j].is_booked && cinema[i][j].booking_code == booking_code) {
                cinema[i][j].is_booked = 0;
                cinema[i][j].booking_code = 0; // Reset codice di prenotazione
                film->available_seats++; // Incrementa il numero di posti disponibili
                printf("Prenotazione annullata con successo.\n");
                found = 1;
            }
        }
    }

    if (!found) {
        printf("Codice di prenotazione non trovato.\n");
    }
}

// Funzione per visualizzare una prenotazione
void view_booking(Seat cinema[][SEATS], Film *film) {
    int booking_code;
    char buffer[N];
    char *endptr;

    printf("Inserisci il codice di prenotazione per visualizzare: ");
    if (!fgets(buffer, N, stdin)) {
        return;
    }
    booking_code = strtol(buffer, &endptr, 10);

    // Verifica se il codice di prenotazione è valido
    if (errno == ERANGE || endptr == buffer || (*endptr && *endptr != '\n')) {
        printf("Codice di prenotazione non valido, riprova.\n");
        return;
    }

    int found = 0;
    for (int i = 0; i < ROWS; i++) {
        if (i == 4) {
            continue;
        }
        for (int j = 0; j < SEATS; j++) {
            if (cinema[i][j].is_booked && cinema[i][j].booking_code == booking_code) {
                printf("Nome del film: %s\n", film->titolo);
                printf("Sala: %d\n", (film == &film1 ? 1 : (film == &film2 ? 2 : 3)));
                printf("Numero di posti: %d\n", film->available_seats);
                printf("Fila: %c, Posto: %d\n", cinema[i][j].row, cinema[i][j].seat_number);
                found = 1;
            }
        }
    }

    if (!found) {
        printf("Codice di prenotazione non trovato.\n");
    }
}

void print_booked_seats(Seat cinema[][SEATS]) {
    printf("\nPosti prenotati:\n");
    for (int i = 0; i < ROWS; i++) {
        if (i == 4) {
            continue;
        }
        for (int j = 0; j < SEATS; j++) {
            if (cinema[i][j].is_booked) {
                printf("Fila %c, Posto %d\n", cinema[i][j].row, cinema[i][j].seat_number);
            }
        }
    }
}

// Funzione per prenotare automaticamente i posti migliori disponibili
void book_best_seat(Seat cinema[][SEATS], Film *film, int num_tickets) {
    int booking_code = rand() % 10000 + 1; // Genera un codice di prenotazione unico
    int booked = 0;

    for (int i = 0; i < ROWS && booked < num_tickets; i++) {
        if (i == 4) {
            continue;
        }
        // Partire dal centro della sala per trovare i posti migliori
        for (int j = SEATS / 2; j >= 0 && booked < num_tickets; j--) {
            if (!cinema[i][j].is_booked && !cinema[i][j].is_disabled) {
                cinema[i][j].is_booked = 1;
                cinema[i][j].booking_code = booking_code;
                film->available_seats--;
                printf("Posto prenotato: Fila %c, Posto %d\n", cinema[i][j].row, cinema[i][j].seat_number);
                booked++;
            }
            if (!cinema[i][SEATS - 1 - j].is_booked && !cinema[i][SEATS - 1 - j].is_disabled && booked < num_tickets) {
                cinema[i][SEATS - 1 - j].is_booked = 1;
                cinema[i][SEATS - 1 - j].booking_code = booking_code;
                film->available_seats--;
                printf("Posto prenotato: Fila %c, Posto %d\n", cinema[i][SEATS - 1 - j].row, cinema[i][SEATS - 1 - j].seat_number);
                booked++;
            }
        }
    }

    if (booked < num_tickets) {
        printf("Non ci sono abbastanza posti disponibili.\n");
    } else {
        printf("Prenotazione completata. Codice di prenotazione: %d\n", booking_code);
    }
}

// Funzione per gestire la scelta tra miglior posto e scelta manuale
void handle_seat_selection(Seat cinema[][SEATS], Film *film, int num_tickets) {
    char choice[N];
    while (1) {
        printf("Scegli l'opzione:\n\n");
        printf("a) Miglior Posto (verrà assegnato il miglior posto disponibile)\n");
        printf("b) Scelta in pianta Posto (scegli manualmente i posti disponibili)\n");
        printf("\nInserisci la tua scelta (a/b): ");
        
        if (!fgets(choice, N, stdin)) {
            continue;
        }

        if (choice[0] == 'a' || choice[0] == 'A') {
            // Assegna automaticamente i posti migliori
            book_best_seat(cinema, film, num_tickets);
            break;
        } else if (choice[0] == 'b' || choice[0] == 'B') {
            // Scelta manuale dei posti
            for (int i = 0; i < num_tickets; i++) {
                printf("Posto numero %d:\n", i + 1);
                display_seats(cinema);
                book_seat(cinema, film);
            }
            break;
        } else {
            printf("Scelta non valida, riprova.\n");
        }
    }
}