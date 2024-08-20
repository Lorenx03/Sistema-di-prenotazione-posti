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

// Inizializzazione dei film
Film film1 = {
    "Fast and Furious",
    "Azione",
    "Inglese",
    130, // Durata in minuti
    "Vin Diesel, Paul Walker, Michelle Rodriguez",
    "Una serie di film che segue le avventure di un gruppo di piloti e ladri in una serie di spettacolari inseguimenti e rapine.",
    "19:00, 21:30",
    ROWS * SEATS - DISABLED_SEATS // Calcola i posti disponibili inizialmente
};

Film film2 = {
    "Titanic",
    "Drammatico/Romantico",
    "Inglese",
    195, // Durata in minuti
    "Leonardo DiCaprio, Kate Winslet, Billy Zane",
    "Una storia d'amore ambientata durante il tragico naufragio del RMS Titanic.",
    "18:00, 20:45",
    ROWS * SEATS - DISABLED_SEATS
};

Film film3 = {
    "Il Piccolo Principe",
    "Animazione/Avventura",
    "Italiano",
    85, // Durata in minuti
    "Voce di: Alessandro Gassmann, Francesca Inaudi",
    "Adattamento del celebre racconto di Antoine de Saint-Exupéry, che segue le avventure di un giovane principe che viaggia attraverso vari pianeti.",
    "17:00, 19:30",
    ROWS * SEATS - DISABLED_SEATS
};

// Creazione delle sale, una per ogni film
Seat cinema1[ROWS][SEATS];  // Sala 1 per Fast and Furious
Seat cinema2[ROWS][SEATS];  // Sala 2 per Titanic
Seat cinema3[ROWS][SEATS];  // Sala 3 per Piccolo Principe

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

int main() {
    // Stampa delle informazioni sui film
    Film films[] = {film1, film2, film3};

    // Inizializzazione delle sale
    initialize_seats(cinema1);
    initialize_seats(cinema2);
    initialize_seats(cinema3);

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
