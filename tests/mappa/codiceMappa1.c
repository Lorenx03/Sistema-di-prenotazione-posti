#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>

#define ROWS 8  // Dalla A alla G con un corridoio (fila vuota)
#define SEATS 9 // 9 posti per fila
#define DISABLED_SEATS 5
#define DISABLED_ROW 5 // Indice 5 corrisponde alla fila E
#define N 100

typedef struct {
    char row;
    int seat_number;
    int is_booked;
    int is_disabled;
} Seat;

void initialize_seats(Seat cinema[][SEATS]);
void display_seats(Seat cinema[][SEATS]);
char get_row_letter(int row_number);
void print_header();
void print_header1();
void book_seat(Seat cinema[][SEATS]);
void convert_to_uppercase(char *str);
void print_booked_seats(Seat cinema[][SEATS]);

int main() {
    Seat cinema[ROWS][SEATS];
    initialize_seats(cinema);
    
    char *films[] = {
        "1. Avengers: Endgame",
        "2. Inception",
        "3. The Matrix",
        "4. Interstellar"
    };
    char *showtimes[] = {
        "1. 10:00 AM",
        "2. 01:00 PM",
        "3. 04:00 PM",
        "4. 07:00 PM"
    };
    int scelta;
    char buffer[N];
    char *endptr;
    int choice;
    char num_tickets[N];

    while (1) {
        printf("\nCinema Booking System\n");
        printf("1. Programmazione\n");
        printf("2. Visualizza posti disponibili\n");
        printf("3. Acquista il tuo biglietto\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        if (!fgets(buffer, N, stdin)) {
            continue;
        }
        choice = strtol(buffer, &endptr, 10);

        if (errno == ERANGE || endptr == buffer || (*endptr && *endptr != '\n') || (choice < 1 || choice > 4)) {
            printf("Invalid choice, please try again.\n");
            continue;
        }

        switch (choice) {
            case 1:
                printf("\nProgrammazione:\n");
                for (int i = 0; i < 4; i++) {
                    printf("%s\n", films[i]);
                }
                break;
            case 2:
                display_seats(cinema);
                break;
            case 3:
                printf("\nProgrammazione:\n");
                for (int i = 0; i < 4; i++) {
                    printf("%s\n", films[i]);
                }
                printf("Scegli il tuo film preferito\n");
                while(1){
                    printf("(Inserisci un numero da 1 a 4 per selezionare il film): ");
                    if (!fgets(buffer, N, stdin)) {
                        continue;
                    }
                    scelta = strtol(buffer, &endptr, 10);

                    if (errno == ERANGE || endptr == buffer || (*endptr && *endptr != '\n') || (scelta < 1 || scelta > 4)) {
                        printf("Scelta non valida, riprova.\n");
                        continue;
                    }
                    break;
                    printf("Hai selezionato: %s\n", films[scelta - 1]);
                }
                /*
                printf("Seleziona l'orario:\n");
                for (int i = 0; i < 4; i++) {
                    printf("%s\n", showtimes[i]);
                }
                
                while(1){
                    printf("(Inserisci un numero da 1 a 4 per selezionare l'orario): ");
                    if (!fgets(buffer, N, stdin)) {
                        continue;
                    }
                    int showtime_choice = strtol(buffer, &endptr, 10);

                    if (errno == ERANGE || endptr == buffer || (*endptr && *endptr != '\n') || (showtime_choice < 1 || showtime_choice > 4)) {
                        printf("Scelta non valida, riprova.\n");
                        continue;
                    }
                    printf("Hai selezionato l'orario: %s\n", showtimes[showtime_choice - 1]);
                    break;
                }
                */
                
                while(1){
                    printf("Seleziona il numero di biglietti da acquistare: ");
                    if (!fgets(num_tickets, N, stdin)) {
                        continue;
                    }
                    int ticket_count = strtol(num_tickets, &endptr, 10);
                    
                    if (errno == ERANGE || endptr == num_tickets || (*endptr && *endptr != '\n') || (ticket_count <= 0)) {
                        printf("Numero di biglietti non valido, riprova.\n");
                        continue;
                    }
                    printf("Devi selezionare %d posti.\n", ticket_count);
                    for (int i = 0; i < ticket_count; i++) {
                        book_seat(cinema);
                    }
                    break;
                }
                
                printf("Acquisto completato!\n");
                print_booked_seats(cinema);
                break;
            case 4:
                exit(0);
            default:
                printf("Invalid choice, please try again.\n");
        }
    }

    return 0;
}

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
        }
    }

    /*
    // Riservare 5 poltrone per disabili nella fila E
    for (int j = 0; j < DISABLED_SEATS; j++) {
        cinema[DISABLED_ROW][j].is_booked = 1;
        cinema[DISABLED_ROW][j].is_disabled = 1;
    }
    */
}

void display_seats(Seat cinema[][SEATS]) {
    print_header();

    for (int i = 0; i < ROWS; i++) {
        // Saltare la fila vuota tra D ed E
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

char get_row_letter(int row_number) {
    // Calcolare la lettera della fila, ignorando la fila vuota
    if (row_number >= 5) {
        return 'A' + row_number - 1;
    } else {
        return 'A' + row_number;
    }
}

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

void book_seat(Seat cinema[][SEATS]) {
    char row[N];
    char seat[N];
    int sceltaPosto;
    int sceltaFila;

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
            printf("Prenotazione effettuata con successo.\n");
        }

        break; // Esce dal ciclo se l'input è valido
    }
}

void convert_to_uppercase(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] >= 'a' && str[i] <= 'z') {
            str[i] = str[i] - 'a' + 'A';
        }
    }
}

void print_booked_seats(Seat cinema[][SEATS]) {
    printf("\nPosti Prenotati:\n");
    for (int i = 0; i < ROWS; i++) {
        // Saltare la fila vuota tra D ed E
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

