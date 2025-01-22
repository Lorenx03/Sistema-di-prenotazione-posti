#include "cinema.h"
#include "filmsCSVparser.h"
#include "utils.h"

void initialize_seats(Hall *hall, int rows, int columns) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            hall->seats[i][j].row = 'A' + i;
            hall->seats[i][j].seat_number = j + 1;
            hall->seats[i][j].state = !(i == (rows-1) && j >= 0 && j <= 2) ? FREE : DISABLED;
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


// ================================ HALL MAP ================================

void generateHallMapResponse(Hall *hall, char *buffer, size_t remaining_size) {
    appendToBuffer(&buffer, &remaining_size, "%d.%d.", hall->rows, hall->columns);

    for (int r = 0; r < hall->rows; r++){
        for(int c = 0; c < hall->columns; c++){
            if (remaining_size > 0) {
                *buffer = '0' + hall->seats[r][c].state;
                buffer++;
                remaining_size--;
            }
        }
    }

    *buffer = '\0';
}
