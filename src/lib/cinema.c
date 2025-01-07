#include "cinema.h"
#include "filmsCSVparser.h"

void initialize_seats(Hall *hall, int rows, int columns) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            hall->seats[i][j].row = 'A' + i;
            hall->seats[i][j].seat_number = j + 1;
            hall->seats[i][j].is_booked = 0;
            hall->seats[i][j].is_disabled = 0;
            hall->seats[i][j].booking_code = 0;
        }
    }
}

void create_halls_for_showtimes(Film *film) {
    film->halls = (Hall*)malloc(film->numbers_showtimes * sizeof(Hall));
    if (!film->halls) {
        fprintf(stderr, "Errore nell'allocazione delle sale\n");
        exit(1);
    }

    for (int i = 0; i < film->numbers_showtimes; i++) {
        film->halls[i].rows = film->rows;
        film->halls[i].columns = film->columns;

        film->halls[i].seats = (Seat **)malloc(film->rows * sizeof(Seat *));
        if (!film->halls[i].seats) {
            fprintf(stderr, "Errore nell'allocazione dei posti\n");
            exit(1);
        }

        for (int j = 0; j < film->rows; j++) {
            film->halls[i].seats[j] = (Seat *)malloc(film->columns * sizeof(Seat));
            if (!film->halls[i].seats[j]) {
                fprintf(stderr, "Errore nell'allocazione dei posti nella fila\n");
                exit(1);
            }
        }
        initialize_seats(&film->halls[i], film->rows, film->columns);
    }
}

void initFilmsList(const char *filename, Films *filmsStruct){
    read_films_csv(filename, &filmsStruct->list, &filmsStruct->count); // Read the CSV file and store the films

    // Inizializza le sale per ogni film
    for (int i = 0; i < filmsStruct->count; i++) {
        filmsStruct->list[i].numbers_showtimes = count_showtimes(&filmsStruct->list[i]);
        create_halls_for_showtimes(&filmsStruct->list[i]);
    }
}


void generateHallMap(Hall *hall, char *buffer, size_t buffer_size) {
    snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "\n");
    for(int w = 0; w <= hall->columns * 2; w++){
        snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), " ");
    }
    snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "   CINEMA   \n     ");
    snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "\n");

    snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "   ");
    for (int j = 1; j <= hall->columns / 3; j++) {
        snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "%3d ", j);
    }
    snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "    ");  // Spazio per il corridoio
    for (int j = hall->columns / 3 + 1; j <= 2 * hall->columns / 3; j++) {
        snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "%3d ", j);
    }
    snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "    ");  // Spazio per il secondo corridoio
    for (int j = 2 * hall->columns / 3 + 1; j <= hall->columns; j++) {
        snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "%3d ", j);
    }
    snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "\n   ");
    for (int j = 1; j <= hall->columns / 3; j++) {
        snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "----");
    }
    snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "----");  // Spazio per il corridoio
    for (int j = hall->columns / 3 + 1; j <= 2 * hall->columns / 3; j++) {
        snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "----");
    }
    snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "----");  // Spazio per il secondo corridoio
    for (int j = 2 * hall->columns / 3 + 1; j <= hall->columns; j++) {
        snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "----");
    }
    snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "\n");
    

    for (int i = 0; i < hall->rows; i++) {
        snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "%c | ", hall->seats[i][0].row);

        // Ciclo per la prima metà dei posti
        for (int j = 0; j < hall->columns / 3; j++) {
            if (hall->seats[i][j].is_disabled) {
                snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "\033[0;34m[D]\033[0m ");
            } else if (hall->seats[i][j].is_booked) {
                snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "\033[0;31m[X]\033[0m ");
            } else {
                snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "\033[0;32m[O]\033[0m ");
            }
        }

        // Corridoio centrale
        snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "    ");  // Spazio centrale per il corridoio

        // Ciclo per la seconda metà dei posti
        for (int j = hall->columns / 3; j < 2 * hall->columns / 3; j++) {
            if (hall->seats[i][j].is_disabled) {
                snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "\033[0;34m[D]\033[0m ");
            } else if (hall->seats[i][j].is_booked) {
                snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "\033[0;31m[X]\033[0m ");
            } else {
                snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "\033[0;32m[O]\033[0m ");
            }
        }

        // Secondo corridoio centrale
        snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "    ");  // Spazio centrale per il secondo corridoio

        // Ciclo per la terza sezione dei posti
        for (int j = 2 * hall->columns / 3; j < hall->columns; j++) {
            if (hall->seats[i][j].is_disabled) {
                snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "\033[0;34m[D]\033[0m ");
            } else if (hall->seats[i][j].is_booked) {
                snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "\033[0;31m[X]\033[0m ");
            } else {
                snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "\033[0;32m[O]\033[0m ");
            }
        }

        snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "| %c\n", hall->seats[i][0].row);
    }

    // Stampa dei trattini
    snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "   ");
    for (int j = 1; j <= hall->columns / 3; j++) {
        snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "----");
    }
    snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "----");
    for (int j = hall->columns / 3 + 1; j <= 2 * hall->columns / 3; j++) {
        snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "----");
    }
    snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "----");
    for (int j = 2 * hall->columns / 3 + 1; j <= hall->columns; j++) {
        snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "----");
    }
    snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "\n");

    // Stampa dei numeri corrispondenti ai posti
    snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "   ");
    for (int j = 1; j <= hall->columns / 3; j++) {
        snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "%3d ", j);
    }
    snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "    ");
    for (int j = hall->columns / 3 + 1; j <= 2 * hall->columns / 3; j++) {
        snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "%3d ", j);
    }
    snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "    ");
    for (int j = 2 * hall->columns / 3 + 1; j <= hall->columns; j++) {
        snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "%3d ", j);
    }
    snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "\n");
}