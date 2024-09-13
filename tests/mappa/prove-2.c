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
#define MAX_LINE_LENGTH 1000
#define MAX_HOURS 10  // Numero massimo di orari per film (puoi adattare questo valore)


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
    int available_seats; // Numero di posti disponibili
} FilmBookings;

// Definizione della struttura Film
typedef struct {
    char titolo[50];
    char genere[30];
    char lingua[20];
    int durata; // Durata in minuti
    char cast[200];
    char trama[500];
    char orari[100];
    short orariDisponibili;
    FilmBookings *filmBookings;
} Film;

// Funzioni di inizializzazione e gestione dei posti
void initialize_seats(Seat halls[][SEATS]);
void display_seats(FilmBookings* hall);
char get_row_letter(int row_number);
void book_seat(Seat halls[][SEATS], Film *film);
void print_header();
void print_header1();
void print_booked_seats(Seat halls[][SEATS]);
void convert_to_uppercase(char *str);
void cancel_booking(Seat halls[][SEATS], Film *film);
void view_booking(Seat halls[][SEATS], Film *film);
FilmBookings* find_hall(FilmBookings* halls, int num_halls, Film* film, const char* showtime);

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

// Funzione per creare le sale per ogni orario del film
FilmBookings* create_halls_halls(Film* films, int num_films, int *num_halls) {
    // Passo 1: Conta il numero totale di orari per determinare il numero di sale
    int total_halls = 0;
    for (int i = 0; i < num_films; i++) {
        char orari_copy[100];
        strncpy(orari_copy, films[i].orari, sizeof(orari_copy) - 1);
        orari_copy[sizeof(orari_copy) - 1] = '\0';  // Sicurezza

        // Conta gli orari separati da virgole
        char* token = strtok(orari_copy, ",");
        while (token != NULL) {
            total_halls++;
            token = strtok(NULL, ",");
        }
    }

    // Passo 2: Allocare memoria per tutte le sale
    FilmBookings* halls = malloc(total_halls * sizeof(FilmBookings));
    if (!halls) {
        perror("Errore nell'allocazione della memoria per le sale");
        return NULL;
    }

    // Passo 3: Creare una sala per ogni orario
    int hall_index = 0;
    for (int i = 0; i < num_films; i++) {
        Film* film = &films[i];
        
        // Copia la stringa degli orari per non modificare l'originale
        char orari_copy[100];
        strncpy(orari_copy, film->orari, sizeof(orari_copy) - 1);
        orari_copy[sizeof(orari_copy) - 1] = '\0';  // Null terminator di sicurezza

        // Tokenizzare la stringa degli orari
        char* token = strtok(orari_copy, ",");
        while (token != NULL) {
            FilmBookings* hall = &halls[hall_index];
            
            // Inizializzare i posti nella sala
            initialize_seats(hall->seats);

            // Salvare l'orario nel campo showtime
            strncpy(hall->showtime, token, sizeof(hall->showtime) - 1);
            hall->showtime[sizeof(hall->showtime) - 1] = '\0';  // Null terminator

            hall_index++;

            // Passa al prossimo orario
            token = strtok(NULL, ",");
        }
    }

    *num_halls = total_halls;
    return halls;
}


// Funzione per inizializzare i posti
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
}

// Funzione per prenotare un posto
void book_seat(Seat halls[][SEATS], Film *film) {
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

        if (halls[row_index][seat_index].is_booked) {
            printf("Il posto è già prenotato.\n");
            continue;
        } else {
            halls[row_index][seat_index].is_booked = 1;
            halls[row_index][seat_index].booking_code = booking_code; // Assegna il codice di prenotazione
            //film->available_seats--; // Decrementa il numero di posti disponibili
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
void cancel_booking(Seat halls[][SEATS], Film *film) {
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
            if (halls[i][j].is_booked && halls[i][j].booking_code == booking_code) {
                halls[i][j].is_booked = 0;
                halls[i][j].booking_code = 0; // Reset codice di prenotazione
                //lable_seats++; // Incrementa il numero di posti disponibili
                printf("Prenotazione annullata con successo.\n");
                found = 1;
            }
        }
    }

    if (!found) {
        printf("Codice di prenotazione non trovato.\n");
    }
}

void print_booked_seats(Seat halls[][SEATS]) {
    printf("\nPosti prenotati:\n");
    for (int i = 0; i < ROWS; i++) {
        if (i == 4) {
            continue;
        }
        for (int j = 0; j < SEATS; j++) {
            if (halls[i][j].is_booked) {
                printf("Fila %c, Posto %d\n", halls[i][j].row, halls[i][j].seat_number);
            }
        }
    }
}

// Funzione per prenotare automaticamente i posti migliori disponibili
void book_best_seat(Seat halls[][SEATS], Film *film, int num_tickets) {
    int booking_code = rand() % 10000 + 1; // Genera un codice di prenotazione unico
    int booked = 0;

    for (int i = 0; i < ROWS && booked < num_tickets; i++) {
        if (i == 4) {
            continue;
        }
        // Partire dal centro della sala per trovare i posti migliori
        for (int j = SEATS / 2; j >= 0 && booked < num_tickets; j--) {
            if (!halls[i][j].is_booked && !halls[i][j].is_disabled) {
                halls[i][j].is_booked = 1;
                halls[i][j].booking_code = booking_code;
                //film->available_seats--;
                printf("Posto prenotato: Fila %c, Posto %d\n", halls[i][j].row, halls[i][j].seat_number);
                booked++;
            }
            if (!halls[i][SEATS - 1 - j].is_booked && !halls[i][SEATS - 1 - j].is_disabled && booked < num_tickets) {
                halls[i][SEATS - 1 - j].is_booked = 1;
                halls[i][SEATS - 1 - j].booking_code = booking_code;
                //film->available_seats--;
                printf("Posto prenotato: Fila %c, Posto %d\n", halls[i][SEATS - 1 - j].row, halls[i][SEATS - 1 - j].seat_number);
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
void handle_seat_selection(Seat halls[][SEATS], Film *film, int num_tickets) {
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
            book_best_seat(halls, film, num_tickets);
            break;
        } else if (choice[0] == 'b' || choice[0] == 'B') {
            // Scelta manuale dei posti
            for (int i = 0; i < num_tickets; i++) {
                printf("Posto numero %d:\n", i + 1);
                display_seats(halls);
                book_seat(halls, film);
            }
            break;
        } else {
            printf("Scelta non valida, riprova.\n");
        }
    }
}

FilmBookings* find_hall(FilmBookings* halls, int num_halls, Film* film, const char* showtime) {
    for (int i = 0; i < num_halls; i++) {
        if (strcmp(halls[i].showtime, showtime) == 0) {
            return &halls[i]; // Restituisce il puntatore alla sala corrispondente
        }
    }
    return NULL; // Se non trova nessuna sala per l'orario selezionato
}


int select_showtime(Film* film) {
    char orari_copy[100];  // Copia temporanea per preservare la stringa originale
    strncpy(orari_copy, film->orari, sizeof(orari_copy) - 1);
    orari_copy[sizeof(orari_copy) - 1] = '\0';  // Null terminator di sicurezza

    char* token = strtok(orari_copy, ", ");  // Tokenizzazione usando la virgola e lo spazio come delimitatori
    int count = 0;  // Contatore degli orari
    char* showtimes[10];  // Array per memorizzare gli orari, con un massimo di 10 orari (adattabile)

    // Step 1: Contare e salvare gli orari
    while (token != NULL && count < 10) {
        showtimes[count] = strdup(token);  // Salva l'orario nel puntatore
        token = strtok(NULL, ", ");  // Prossimo orario
        count++;
    }

    // Step 2: Mostrare gli orari all'utente
    printf("\nSeleziona l'orario:\n");
    for (int i = 0; i < count; i++) {
        printf("%d) %s\n", i + 1, showtimes[i]);
    }

    // Step 3: Far scegliere all'utente usando fgets
    char buffer[10];
    int showtime_choice;
    char* endptr;

    do {
        printf("\nInserisci il numero dell'orario (1-%d): ", count);
        if (!fgets(buffer, sizeof(buffer), stdin)) {
            printf("Errore nella lettura dell'input.\n");
            continue;
        }
        showtime_choice = strtol(buffer, &endptr, 10);

        if (errno == ERANGE || endptr == buffer || (*endptr && *endptr != '\n') || (showtime_choice < 1 || showtime_choice > count)) {
            printf("Scelta non valida, riprova.\n");
            continue;
        }
    } while (showtime_choice < 1 || showtime_choice > count);

    // Step 4: Stampa dell'orario selezionato
    printf("Hai selezionato l'orario: %s\n", showtimes[showtime_choice - 1]);

    // Step 5: Pulizia della memoria allocata
    for (int i = 0; i < count; i++) {
        free(showtimes[i]);
    }

    // Restituisci l'indice dell'orario selezionato (0-based)
    return showtime_choice - 1;
}




int main() {
    // Passo 1: Carica i film dal file CSV
    int num_films = 0;
    Film* films = parse_csv_film("films.csv", &num_films);
    if (!films) {
        fprintf(stderr, "Errore nel caricamento dei film.\n");
        return 1;
    }

    // Passo 2: Crea le sale hallstografiche
    int num_halls = 0;
    FilmBookings* halls = create_halls_halls(films, num_films, &num_halls);
    if (!halls) {
        fprintf(stderr, "Errore nella creazione delle sale.\n");
        free(films);
        return 1;
    }

    srand(time(NULL)); // Inizializza il generatore di numeri casuali

    int choice;
    char buffer[N];
    char *endptr;

    while (1) {
        printf("\nhalls\n\n");
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
                while (1) {
                    printf("\nProgrammazione:\n");

                    for (int i = 0; i < num_films; i++) {
                        printf("Titolo: %s\n", films[i].titolo);
                        printf("Orari: %s\n", films[i].orari);
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
                        printf("\nSeleziona il film di cui vuoi sapere i dettagli: \n");
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
                printf("\nBiglietteria\n");
                printf("\nLIMITI DI ACQUISTO: In fase di acquisto ogni cliente potrà acquistare fino ad un massimo di 4 biglietti.\nLa disponibilità dei posti è limitata.\n");
                                
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
                printf("\nSeleziona l'orario: %s\n", films[film_choice - 1].orari);

                // Chiedi all'utente di selezionare un orario
                int selected_showtime = select_showtime(&films[film_choice - 1]);

                // Trova l'indice della sala corrispondente all'orario selezionato
                FilmBookings* selected_hall = find_hall(halls, num_halls, &films[film_choice - 1], films[film_choice - 1].orari);

                if (selected_hall != NULL) {
                    // Visualizza la sala corrispondente all'orario selezionato
                    printf("Visualizzazione della sala per l'orario selezionato...\n");
                    display_seats(selected_hall);

                    // Gestione della selezione dei posti
                    int num_tickets;
                    while (1) {
                        printf("Quanti biglietti desideri acquistare? (Max 4): ");
                        if (!fgets(buffer, N, stdin)) {
                            printf("Errore nell'input.\n");
                            continue;
                        }
                        num_tickets = strtol(buffer, &endptr, 10);

                        if (errno == ERANGE || endptr == buffer || (*endptr && *endptr != '\n') || num_tickets < 1 || num_tickets > 4) {
                            printf("Numero di biglietti non valido, riprova.\n");
                            continue;
                        }
                        break;
                    }

                    // Gestione della selezione dei posti
                    handle_seat_selection(selected_hall->seats, &films[film_choice - 1], num_tickets);
                } else {
                    printf("Orario non trovato per il film selezionato.\n");
                }

                break;
                

            case 4:
                // Esci dal programma
                printf("Uscita dal programma...\n");
                exit(0);
                break;
        }
    }

    free(halls);
    free(films);

    return 0;
}
