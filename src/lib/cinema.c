#include "cinema.h"
#include "filmsCSVparser.h"
#include "utils.h"

void initialize_seats(Hall *hall, int rows, int columns) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            hall->seats[i][j].row = 'A' + i;
            hall->seats[i][j].seat_number = j + 1;
            hall->seats[i][j].state = !(i == (rows-1) && j >= 0 && j <= 2) ? FREE : DISABLED;
            memset(hall->seats[i][j].booking_code, 0, sizeof(hall->seats[i][j].booking_code));
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

// ================================ BOOKING ================================

int bookSeat(Hall *hall, char *seat, char *bookingCode) {
    if(hall == NULL || seat == NULL){
        fprintf(stderr, "bookSeat: NULL pointer\n");
        return 1;
    }

    //Check if the seat has the correct format
    if (strlen(seat) < 2 || strlen(seat) > 3) {
        fprintf(stderr, "bookSeat: Invalid seat format\n");
        return 1;
    }

    int row = seat[0] - 'A';
    int column = safeStrToInt(&seat[1]) - 1;

    if (row < 0 || row >= hall->rows || column < 0 || column > hall->columns) {
        fprintf(stderr, "bookSeat: Invalid seat\n");
        return 1;
    }

    if (hall->seats[row][column].state == FREE) {
        hall->seats[row][column].state = BOOKED;
        strncpy(hall->seats[row][column].booking_code, bookingCode, sizeof(hall->seats[row][column].booking_code));
        return 0;
    }else{
        fprintf(stderr, "bookSeat: Seat already booked\n");
        return 1;
    }
}


void printTicket(char **buff, char *bookingCode, char *filmTitle, char *filmShowtime, char *seat, size_t *remaining_size){
    appendToBuffer(buff, remaining_size, "============== BIGLIETTO =============\n");
    appendToBuffer(buff, remaining_size, "Codice prenotazione: %s\n", bookingCode);
    appendToBuffer(buff, remaining_size, "Film: %s\n", filmTitle);
    appendToBuffer(buff, remaining_size, "Orario: %s\n", filmShowtime);
    appendToBuffer(buff, remaining_size, "Posto: %s\n", seat);
    appendToBuffer(buff, remaining_size, "======================================\n\n\n");
}