#include <stdio.h>
#include "cinema.h"

// Inizializzazione della mappa dei posti
void initialize_seats(Hall *hall) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < SEATS; j++) {
            hall->seats[i][j].row = get_row_letter(i);
            hall->seats[i][j].seat_number = j + 1;
            hall->seats[i][j].is_booked = 0;
            hall->seats[i][j].is_disabled = 0;
            hall->seats[i][j].booking_code = 0;
        }
    }

    // Riservare i posti per disabili nella fila E (DISABLED_ROW)
    for (int j = 0; j < DISABLED_SEATS; j++) {
        hall->seats[DISABLED_ROW][j].is_disabled = 1;
    }
}

// Funzione per ottenere la lettera della riga
char get_row_letter(int row_number) {
    return 'A' + row_number;
}

// Funzione per visualizzare i posti
void display_seats(Hall *hall) {
    print_header2();
    print_header();
    for (int i = 0; i < ROWS; i++) {
        printf("%c | ", hall->seats[i][0].row);

        // Ciclo per la prima metà dei posti
        for (int j = 0; j < SEATS / 3; j++) {
            if (hall->seats[i][j].is_disabled) {
                printf("\033[0;34m[D]\033[0m ");
            } else if (hall->seats[i][j].is_booked) {
                printf("\033[0;31m[X]\033[0m ");
            } else {
                printf("\033[0;32m[O]\033[0m ");
            }
        }

        // Corridoio centrale
        printf("    ");  // Spazio centrale per il corridoio

        // Ciclo per la seconda metà dei posti
        for (int j = SEATS / 3; j < 2 * SEATS / 3; j++) {
            if (hall->seats[i][j].is_disabled) {
                printf("\033[0;34m[D]\033[0m ");
            } else if (hall->seats[i][j].is_booked) {
                printf("\033[0;31m[X]\033[0m ");
            } else {
                printf("\033[0;32m[O]\033[0m ");
            }
        }

        // Secondo corridoio centrale
        printf("    ");  // Spazio centrale per il secondo corridoio

        // Ciclo per la terza sezione dei posti
        for (int j = 2 * SEATS / 3; j < SEATS; j++) {
            if (hall->seats[i][j].is_disabled) {
                printf("\033[0;34m[D]\033[0m ");
            } else if (hall->seats[i][j].is_booked) {
                printf("\033[0;31m[X]\033[0m ");
            } else {
                printf("\033[0;32m[O]\033[0m ");
            }
        }

        printf("| %c\n", hall->seats[i][0].row);

        // Divisione tra le prime 5 file e le ultime 5 con corridoio centrale
        if (ROWS > 5 && i == ROWS_BEFORE_CORRIDOR - 1) {
            printf("\n\n");  // Aggiungi uno spazio tra la 5a e la 6a fila
        }
    }
    print_footer();
    printf("\nLegenda: \n\033[0;32m[O]\033[0m Disponibile \n\033[0;31m[X]\033[0m Prenotato \n\033[0;34m[D]\033[0m Riservato per disabili\n\n");
}

// Funzione per stampare l'intestazione della visualizzazione dei posti
void print_header() {
    printf("   ");
    for (int j = 1; j <= SEATS / 3; j++) {
        printf("%3d ", j);
    }
    printf("    ");  // Spazio per il corridoio
    for (int j = SEATS / 3 + 1; j <= 2 * SEATS / 3; j++) {
        printf("%3d ", j);
    }
    printf("    ");  // Spazio per il secondo corridoio
    for (int j = 2 * SEATS / 3 + 1; j <= SEATS; j++) {
        printf("%3d ", j);
    }
    printf("\n   ");
    for (int j = 1; j <= SEATS / 3; j++) {
        printf("----");
    }
    printf("----");  // Spazio per il corridoio
    for (int j = SEATS / 3 + 1; j <= 2 * SEATS / 3; j++) {
        printf("----");
    }
    printf("----");  // Spazio per il secondo corridoio
    for (int j = 2 * SEATS / 3 + 1; j <= SEATS; j++) {
        printf("----");
    }
    printf("\n");
}

// Funzione per stampare il piè di pagina della visualizzazione dei posti
void print_footer() {
    // Stampa dei trattini
    printf("   ");
    for (int j = 1; j <= SEATS / 3; j++) {
        printf("----");
    }
    printf("----");
    for (int j = SEATS / 3 + 1; j <= 2 * SEATS / 3; j++) {
        printf("----");
    }
    printf("----");
    for (int j = 2 * SEATS / 3 + 1; j <= SEATS; j++) {
        printf("----");
    }
    printf("\n");

    // Stampa dei numeri corrispondenti ai posti
    printf("   ");
    for (int j = 1; j <= SEATS / 3; j++) {
        printf("%3d ", j);
    }
    printf("    ");
    for (int j = SEATS / 3 + 1; j <= 2 * SEATS / 3; j++) {
        printf("%3d ", j);
    }
    printf("    ");
    for (int j = 2 * SEATS / 3 + 1; j <= SEATS; j++) {
        printf("%3d ", j);
    }
    printf("\n");
}

void print_header2() {
    printf("\n");
    for(int w = 0; w <= SEATS * 2; w++){
        printf(" ");
    }
    printf("   CINEMA   \n     ");
    printf("\n");
}
