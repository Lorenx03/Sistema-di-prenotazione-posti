#include "cinema.h"
#include "filmsCSVparser.h"

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

void appendToBuffer(char **buffer_ptr, size_t *remaining_size, const char *format, ...) {
    va_list args;
    va_start(args, format);
    int written = vsnprintf(*buffer_ptr, *remaining_size, format, args);
    va_end(args);

    if (written > 0) {
        *buffer_ptr += written;
        *remaining_size -= written;
    }
}

void drawSeatNumbers(char **buffer, size_t *remaining_size, const int columns) {
    appendToBuffer(buffer, remaining_size, "    ");
    for (int j = 1; j <= columns; j++) {
        if (j<9)
            appendToBuffer(buffer, remaining_size, "%3d  ", j);
        else
            appendToBuffer(buffer, remaining_size, "%3d   ", j);
    }
    appendToBuffer(buffer, remaining_size, "\n");
}

void drawSeparatorLine(char **buffer, size_t *remaining_size, const int columns) {
    appendToBuffer(buffer, remaining_size, "  --");
    for (int j = 1; j <= columns; j++) {
        if (j<9)
            appendToBuffer(buffer, remaining_size, "-----");
        else
            appendToBuffer(buffer, remaining_size, "------");
    }
    appendToBuffer(buffer, remaining_size, "\n");
}

void generateHallMap(Hall *hall, char *buffer, size_t remaining_size) {
    appendToBuffer(&buffer, &remaining_size, "%d.%d", hall->rows, hall->columns);

    // CINEMA
    appendToBuffer(&buffer, &remaining_size, "\n");
    for(int w = 0; w <= hall->columns * 13/5; w++){
        appendToBuffer(&buffer, &remaining_size," ");
    }
    appendToBuffer(&buffer, &remaining_size, "CINEMA\n\n");

    drawSeatNumbers(&buffer, &remaining_size, hall->columns);
    drawSeparatorLine(&buffer, &remaining_size, hall->columns);

    // Seats
    for (int i = 0; i < hall->rows; i++) {
        appendToBuffer(&buffer, &remaining_size, "%c | ", hall->seats[i][0].row);

        for (int j = 0; j < hall->columns; j++) {
            switch (hall->seats[i][j].state){
                case FREE:
                    appendToBuffer(&buffer, &remaining_size, "\033[0;32m[%c%d]\033[0m ", hall->seats[i][0].row, j+1);
                    break;
                case BOOKED:
                    appendToBuffer(&buffer, &remaining_size, "\033[0;31m[%c%d]\033[0m ", hall->seats[i][0].row, j+1);
                    break;
                case DISABLED:
                    appendToBuffer(&buffer, &remaining_size, "\033[0;34m[%c%d]\033[0m ", hall->seats[i][0].row, j+1);
                    break;

                default:
                    break;
            }
        }

        appendToBuffer(&buffer, &remaining_size, "| %c\n", hall->seats[i][0].row);

        if(i == hall->rows/3){
            appendToBuffer(&buffer, &remaining_size, "  |");
            for (int j = 1; j <= hall->columns; j++) {
                if (j<9)
                    appendToBuffer(&buffer, &remaining_size, "     ");
                else
                    appendToBuffer(&buffer, &remaining_size, "      ");
            }
            appendToBuffer(&buffer, &remaining_size, "|\n");
        }
    }

    drawSeparatorLine(&buffer, &remaining_size, hall->columns);
    drawSeatNumbers(&buffer, &remaining_size, hall->columns);

    // Legend
    appendToBuffer(&buffer, &remaining_size, "\nLegenda: \n\033[0;32m[A1]\033[0m Disponibile \n\033[0;31m[A1]\033[0m Prenotato \n\033[0;34m[A1]\033[0m Disabili\n");
}
