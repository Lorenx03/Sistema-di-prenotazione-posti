#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>

#define ROWS 8  // Dalla A alla G con un corridoio (fila vuota)
#define SEATS 9 // 9 posti per fila
#define DISABLED_SEATS 5
#define DISABLED_ROW 4 // Indice 4 corrisponde alla fila E
#define N 100
#define MAX_TICKETS 4
#define MAX_LINE_LENGTH 512

typedef struct {
    char row;
    int seat_number;
    int is_booked;
    int is_disabled;
    int booking_code; // Codice unico per la prenotazione
} Seat;

typedef struct {
    Seat seats[ROWS][SEATS]; // Matrice dei posti
    char showtime[20];       // Orario della proiezione
    int available_seats;     // Numero di posti disponibili
} FilmBookings;

typedef struct {
    char titolo[50];
    char genere[30];
    char lingua[20];
    int durata; 
    char cast[200];
    char trama[500];
    char orari[100]; // Orari come stringa, separati da virgola
    FilmBookings bookings_per_showtime[10]; // Prenotazioni per ciascun orario
    int num_showtimes; // Numero di orari disponibili
} Film;

// Funzioni di inizializzazione e gestione dei posti
void initialize_seats(Seat halls[][SEATS]);
void display_seats(FilmBookings* hall);
char get_row_letter(int row_number);
void book_seat(Seat halls[][SEATS], Film *film);
void print_header();
void print_header1();
void convert_to_uppercase(char *str);
void handle_seat_selection(Seat halls[][SEATS], Film *film, int num_tickets);
void book_best_seat(Seat halls[][SEATS], Film *film, int num_tickets);

Film* parse_csv_film(const char* filename, int* num_films);
void initialize_film_bookings(Film *film);
void display_film_seats(Film *film, int showtime_index);

// Funzione per analizzare il file CSV e caricare i dati dei film
Film* parse_csv_film(const char* filename, int* num_films) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Errore nell'apertura del file");
        return NULL;
    }

    Film* films = NULL;
    int capacity = 10;
    int count = 0;
    char line[MAX_LINE_LENGTH];

    films = malloc(capacity * sizeof(Film));
    if (!films) {
        fclose(file);
        return NULL;
    }

    while (fgets(line, sizeof(line), file)) {
        if (count >= capacity) {
            capacity *= 2;
            Film* temp = realloc(films, capacity * sizeof(Film));
            if (!temp) {
                free(films);
                fclose(file);
                return NULL;
            }
            films = temp;
        }

        Film film = {0};
        char* token;
        char* rest = line;
        int field = 0;
        bool in_quotes = false;
        char buffer[MAX_LINE_LENGTH] = {0};
        int buffer_pos = 0;

        while ((token = strtok_r(rest, ",", &rest))) {
            for (int i = 0; token[i]; i++) {
                if (token[i] == '"') {
                    in_quotes = !in_quotes;
                } else {
                    if (buffer_pos < sizeof(buffer) - 1) {
                        buffer[buffer_pos++] = token[i];
                    }
                }
            }

            if (!in_quotes) {
                buffer[buffer_pos] = '\0';
                switch (field) {
                    case 0: strncpy(film.titolo, buffer, sizeof(film.titolo) - 1); break;
                    case 1: strncpy(film.genere, buffer, sizeof(film.genere) - 1); break;
                    case 2: strncpy(film.lingua, buffer, sizeof(film.lingua) - 1); break;
                    case 3: film.durata = atoi(buffer); break;
                    case 4: strncpy(film.cast, buffer, sizeof(film.cast) - 1); break;
                    case 5: strncpy(film.trama, buffer, sizeof(film.trama) - 1); break;
                    case 6: strncpy(film.orari, buffer, sizeof(film.orari) - 1); break;
                }
                field++;
                buffer_pos = 0;
            } else {
                buffer[buffer_pos++] = ',';
            }
        }

        films[count++] = film;
    }

    fclose(file);
    *num_films = count;
    return films;
}

// Funzione per inizializzare i posti in una sala
void initialize_seats(Seat halls[][SEATS]) {
    for (int i = 0; i < ROWS; i++) {
        // Saltare la fila vuota tra D ed E
        if (i == 4) {
            continue;
        }
        for (int j = 0; j < SEATS; j++) {
            halls[i][j].row = get_row_letter(i);
            halls[i][j].seat_number = j + 1;
            halls[i][j].is_booked = 0;
            halls[i][j].is_disabled = 0;
            halls[i][j].booking_code = 0;
        }
    }

    // Riservare 5 poltrone per disabili nella fila E
    for (int j = 0; j < DISABLED_SEATS; j++) {
        halls[DISABLED_ROW][j].is_disabled = 1;
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
void display_seats(FilmBookings* hall) {
    print_header();
    for (int i = 0; i < ROWS; i++) {
        if (i == DISABLED_ROW) {
            printf("\n");  // Spazio vuoto per il corridoio
            continue;
        }
        printf("%c | ", hall->seats[i][0].row);
        for (int j = 0; j < SEATS; j++) {
            if (hall->seats[i][j].is_disabled) {
                printf("\033[0;34m[D]\033[0m "); // Blu per disabili
            } else if (hall->seats[i][j].is_booked) {
                printf("\033[0;31m[X]\033[0m "); // Rosso per prenotato
            } else {
                printf("\033[0;32m[O]\033[0m "); // Verde per disponibile
            }
        }
        printf("| %c\n", hall->seats[i][0].row);
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
    printf("\n");
}

void handle_seat_selection(Seat halls[][SEATS], Film *film, int num_tickets) {
    display_seats(&film->bookings_per_showtime[0]); // visualizza i posti disponibili
    printf("Hai scelto di prenotare %d posti.\n", num_tickets);
    
    if (num_tickets > MAX_TICKETS) {
        printf("Puoi prenotare un massimo di %d biglietti per volta.\n", MAX_TICKETS);
        return;
    }

    printf("Vuoi selezionare manualmente i posti o assegnarli automaticamente?\n");
    printf("1. Manuale\n");
    printf("2. Automatico\n");
    int scelta;
    scanf("%d", &scelta);

    if (scelta == 1) {
        for (int i = 0; i < num_tickets; i++) {
            printf("Seleziona il posto %d.\n", i + 1);
            printf("Inserisci la riga (lettera): ");
            char row_letter;
            scanf(" %c", &row_letter);
            row_letter = toupper(row_letter);

            printf("Inserisci il numero del posto: ");
            int seat_number;
            scanf("%d", &seat_number);

            int row = (row_letter >= 'A' && row_letter <= 'D') ? row_letter - 'A' : row_letter - 'A' + 1;

            if (halls[row][seat_number - 1].is_booked) {
                printf("Il posto %c%d è già prenotato. Seleziona un altro posto.\n", row_letter, seat_number);
                i--; // Ripeti questo ciclo per permettere una nuova selezione
            } else {
                // Prenota il posto
                halls[row][seat_number - 1].is_booked = 1;
                halls[row][seat_number - 1].booking_code = rand() % 9000 + 1000; // Codice di prenotazione casuale
                printf("Hai prenotato il posto %c%d. Codice di prenotazione: %d\n", row_letter, seat_number, halls[row][seat_number - 1].booking_code);
            }
        }
    } else if (scelta == 2) {
        // Seleziona automaticamente i migliori posti disponibili
        book_best_seat(halls, film, num_tickets);
    } else {
        printf("Scelta non valida.\n");
    }

    // Mostra nuovamente i posti dopo la prenotazione
    display_seats(&film->bookings_per_showtime[0]);
}

// Funzione per prenotare automaticamente i migliori posti disponibili
void book_best_seat(Seat halls[][SEATS], Film *film, int num_tickets) {
    printf("Selezione automatica dei posti migliori...\n");

    int booked_count = 0;
    for (int i = 0; i < ROWS && booked_count < num_tickets; i++) {
        if (i == DISABLED_ROW) continue; // Salta la fila disabili

        for (int j = 0; j < SEATS && booked_count < num_tickets; j++) {
            if (!halls[i][j].is_booked && !halls[i][j].is_disabled) {
                halls[i][j].is_booked = 1;
                halls[i][j].booking_code = rand() % 9000 + 1000; // Codice di prenotazione casuale
                printf("Hai prenotato il posto %c%d. Codice di prenotazione: %d\n", get_row_letter(i), j + 1, halls[i][j].booking_code);
                booked_count++;
            }
        }
    }

    if (booked_count < num_tickets) {
        printf("Non ci sono abbastanza posti disponibili.\n");
    }
}

// Funzione per inizializzare le prenotazioni per ciascun orario del film
void initialize_film_bookings(Film *film) {
    // Converti gli orari del film in un array di orari
    char *token = strtok(film->orari, ",");
    int index = 0;

    while (token != NULL && index < 10) {
        strncpy(film->bookings_per_showtime[index].showtime, token, sizeof(film->bookings_per_showtime[index].showtime) - 1);
        initialize_seats(film->bookings_per_showtime[index].seats); // Inizializza i posti per ciascun orario
        film->bookings_per_showtime[index].available_seats = (ROWS - 1) * SEATS - DISABLED_SEATS;
        token = strtok(NULL, ",");
        index++;
    }

    film->num_showtimes = index; // Numero di orari del film
}

// Funzione per visualizzare i posti disponibili per un determinato orario
void display_film_seats(Film *film, int showtime_index) {
    if (showtime_index >= 0 && showtime_index < film->num_showtimes) {
        printf("Orario: %s\n", film->bookings_per_showtime[showtime_index].showtime);
        display_seats(&film->bookings_per_showtime[showtime_index]);
    } else {
        printf("Orario non valido.\n");
    }
}

// Funzione per convertire una stringa in maiuscolo
void convert_to_uppercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper(str[i]);
    }
}

int main() {
    // Passo 1: Carica i film dal file CSV
    int num_films;
    Film *films = parse_csv_film("films.csv", &num_films);

    // Inizializza le prenotazioni per ogni film
    for (int i = 0; i < num_films; i++) {
        initialize_film_bookings(&films[i]);
    }

    srand(time(NULL)); // Inizializza il generatore di numeri casuali

    int choice;
    char buffer[N];
    char *endptr;

    while (1) {
        printf("\n CINEMA \n\n");
        printf("1. Programmazione\n");
        printf("2. Biglietteria\n");
        printf("3. Ordini e Prenotazioni\n");
        printf("4. Esci\n");

        printf("\nInserisci la tua scelta: ");
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
                // Programmazione
                while (1) {
                    printf("\n PROGRAMMAZIONE \n\n");
                    for (int i = 0; i < num_films; i++) {
                        printf("Film %d: %s\n", i + 1, films[i].titolo);
                        printf("\n");
                    }

                    printf("1. Visualizza dettagli di un film specifico\n");
                    printf("2. Torna al menu principale\n");
                    printf("\nScegli un'opzione (1-2): ");

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
                        printf("\nSeleziona il film di cui vuoi sapere i dettagli: \n\n");
                        for (int i = 0; i < num_films; i++) {
                            printf("Film %d: %s\n", i + 1, films[i].titolo);
                        }
                        
                        int film_choice;
                        while (1) {
                            if (!fgets(buffer, N, stdin)) {
                                continue;
                            }
                            film_choice = strtol(buffer, &endptr, 10);

                            if (errno == ERANGE || endptr == buffer || (*endptr && *endptr != '\n') || (film_choice < 1 || film_choice > num_films)) {
                                printf("Scelta non valida, riprova.\n");
                                continue;
                            }
                            break;
                        }

                        printf("\nDettagli del film selezionato:\n\n");
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
                // Biglietteria
                printf("\n BIGLIETTERIA \n");
                printf("\nLIMITI DI ACQUISTO: Ogni cliente può acquistare fino a un massimo di 4 biglietti.\n");

                printf("\nProgrammazione:\n\n");
                for (int i = 0; i < num_films; i++) {
                    printf("%d - %s\n", i + 1, films[i].titolo);
                }

                printf("\nSeleziona il Film: ");
                int film_choice;
                while (1) {
                    if (!fgets(buffer, N, stdin)) {
                        continue;
                    }
                    film_choice = strtol(buffer, &endptr, 10);

                    if (errno == ERANGE || endptr == buffer || (*endptr && *endptr != '\n') || (film_choice < 1 || film_choice > num_films)) {
                        printf("Scelta non valida, riprova.\n");
                        continue;
                    }
                    break;
                }

                printf("\nHai selezionato: %s\n", films[film_choice - 1].titolo);

                // Seleziona l'orario
                printf("\nSeleziona l'orario:\n");
                for (int i = 0; i < films[film_choice - 1].num_showtimes; i++) {
                    printf("%d - %s\n", i + 1, films[film_choice - 1].bookings_per_showtime[i].showtime);
                }

                int showtime_choice;
                while (1) {
                    if (!fgets(buffer, N, stdin)) {
                        continue;
                    }
                    showtime_choice = strtol(buffer, &endptr, 10);

                    if (errno == ERANGE || endptr == buffer || (*endptr && *endptr != '\n') || (showtime_choice < 1 || showtime_choice > films[film_choice - 1].num_showtimes)) {
                        printf("Scelta non valida, riprova.\n");
                        continue;
                    }
                    break;
                }

                // Seleziona il numero di biglietti
                printf("\nInserisci il numero di biglietti (1-4): ");
                int num_tickets;
                while (1) {
                    if (!fgets(buffer, N, stdin)) {
                        continue;
                    }
                    num_tickets = strtol(buffer, &endptr, 10);

                    if (errno == ERANGE || endptr == buffer || (*endptr && *endptr != '\n') || (num_tickets < 1 || num_tickets > MAX_TICKETS)) {
                        printf("Numero di biglietti non valido, riprova.\n");
                        continue;
                    }
                    break;
                }

                // Gestisci la selezione dei posti
                handle_seat_selection(films[film_choice - 1].bookings_per_showtime[showtime_choice - 1].seats, &films[film_choice - 1], num_tickets);

                break;

            case 3:
                // Ordini e Prenotazioni (da implementare)
                printf("\nOrdini e Prenotazioni\n");
                break;

            case 4:
                // Esci dal programma
                printf("Uscita dal programma...\n");
                free(films);
                exit(0);
                break;
        }
    }

    return 0;
}
