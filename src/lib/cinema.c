#include "cinema.h"
#include "filmsCSVparser.h"
#include "utils.h"

void initialize_seats(Hall *hall, int rows, int columns) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            hall->seats[i][j].row = 'A' + i;
            hall->seats[i][j].seat_number = j + 1;
            hall->seats[i][j].state = !(i == (rows-1) && j >= 0 && j <= 2) ? FREE : DISABLED;
            pthread_mutex_init(&hall->seats[i][j].lock, NULL);
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

void freeFilmsList(Films *filmsStruct) {
    if (filmsStruct->list == NULL) {
        fprintf(stderr, "freeFilmsList: filmsStruct->list is NULL\n");
        return;
    }

    for (int i = 0; i < filmsStruct->count; i++) {
        Film *film = &filmsStruct->list[i];
        for (int s = 0; s < film->numbers_showtimes; s++) {
            Hall *hall = &film->halls[s];
            for (int r = 0; r < hall->rows; r++) {
                for (int c = 0; c < hall->columns; c++) {
                    pthread_mutex_destroy(&hall->seats[r][c].lock);
                }
                free(hall->seats[r]);
            }
            free(hall->seats);
        }
        free(film->halls);
        free(film->name);
        free(film->genre);
        free(film->language);
        free(film->actors);
        free(film->plot);
        free(film->showtimes);
    }

    free(filmsStruct->list);
    filmsStruct->list = NULL;
    filmsStruct->count = 0;
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

// =============================== BOOKING =================================

int bookSeats(Hall *hall, int numSeats, int seats[numSeats][2], char bookingCodes[numSeats][18]) {
    if(hall == NULL || seats == NULL || bookingCodes == NULL || numSeats <= 0 || numSeats > 4){
        fprintf(stderr, "bookSeat: ERROR -  Bad parameters\n");
        return 1;
    }

    for (int i = 0; i < numSeats; i++){
        // Lock the seat, we use trylock to avoid blocking, if the seat is already locked it means that another thread is booking it
        if(
            pthread_mutex_trylock(&hall->seats[seats[i][0]][seats[i][1]].lock) != 0 ||
            hall->seats[seats[i][0]][seats[i][1]].state == BOOKED
        ){
            fprintf(stderr, "bookSeat: ERROR - Seat being booked or already boocked\n");
            for (int j = 0; j < i; i++){
                pthread_mutex_unlock(&hall->seats[seats[j][0]][seats[j][1]].lock);
            }
            return 1;
        }
    }

    SeatState temp[4] = {FREE, FREE, FREE, FREE};
    // Now that we locked the seats, we can book them
    for (int i = 0; i < numSeats; i++) {
        // Check if seat coordinates are valid
        if (seats[i] == NULL || 
            seats[i][0] < 0 || 
            seats[i][0] >= hall->rows ||
            seats[i][1] < 0 || seats[i][1] >= hall->columns
        ) {
            fprintf(stderr, "bookSeat: Invalid seat coordinates\n");

            // Reset the seats that we booked before the error
            for (int j = 0; j < i; j++){
                hall->seats[seats[j][0]][seats[j][1]].state = temp[j];
                memset(hall->seats[seats[j][0]][seats[j][1]].booking_code, 0, sizeof(hall->seats[seats[j][0]][seats[j][1]].booking_code));
                pthread_mutex_unlock(&hall->seats[seats[j][0]][seats[j][1]].lock);
            }
            return 1;
        }

        temp[i] = hall->seats[seats[i][0]][seats[i][1]].state;
        hall->seats[seats[i][0]][seats[i][1]].state = BOOKED;
        strncpy(hall->seats[seats[i][0]][seats[i][1]].booking_code, bookingCodes[i], sizeof(hall->seats[seats[i][0]][seats[i][1]].booking_code));

        pthread_mutex_unlock(&hall->seats[seats[i][0]][seats[i][1]].lock);
    }

    return 0;
}


// TODO: Change this
int saveBookingsToFile(Films *filmsStruct, const char *filename){
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    for (int i = 0; i < filmsStruct->count; i++) {
        for (int j = 0; j < filmsStruct->list[i].numbers_showtimes; j++) {
            for (int r = 0; r < filmsStruct->list[i].rows; r++) {
                for (int c = 0; c < filmsStruct->list[i].columns; c++) {
                    if (filmsStruct->list[i].halls[j].seats[r][c].state == BOOKED) {
                        // Again we use trylock, not to avoid blocking but if the lock is locked it means that the seat is being booked, and thus is in a non-consistent state, so we skip it to check on it later.
                        if (pthread_mutex_trylock(&filmsStruct->list[i].halls[j].seats[r][c].lock) == 0) {
                            fprintf(file, "%d.%d.%c%d.%s\n", i, j, 'A' + r, c + 1, filmsStruct->list[i].halls[j].seats[r][c].booking_code);
                            pthread_mutex_unlock(&filmsStruct->list[i].halls[j].seats[r][c].lock);
                        }
                    }
                }
            }
        }
    }

    fflush(file);
    fclose(file);
    return 0;
}

int loadBookingsFromFile(Films *filmsStruct, const char *filename){
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    char line[1024] = {0};
    char *token;
    char *saveptr;
    int film_id, showtime_id;
    int seat[1][2];
    char booking_code[1][18] = {0};

    int i = 1;
    while (fgets(line, sizeof(line), file) != NULL) {
        token = strtok_r(line, ".", &saveptr);
        i = 1;
        while (token != NULL && i <= 4) {
            switch (i){
                case 1:
                    film_id = safeStrToInt(token);
                    if (film_id < 0 || film_id >= filmsStruct->count){
                        fprintf(stderr, "loadBookingsFromFile: Invalid film id\n");
                        fclose(file);
                        return i;
                    }
                break;

                case 2:
                    showtime_id = safeStrToInt(token);
                    if (showtime_id < 0 || showtime_id >= filmsStruct->list[film_id].numbers_showtimes){
                        fprintf(stderr, "loadBookingsFromFile: Invalid showtime id\n");
                        fclose(file);
                        return i;
                    }
                break;

                case 3:
                    if (strlen(token) < 2 || strlen(token) > 3){
                        fprintf(stderr, "loadBookingsFromFile: Invalid seat\n");
                        fclose(file);
                        return i;
                    }

                    seat[0][0] = token[0] - 'A';
                    seat[0][1] = safeStrToInt(&token[1]) - 1;

                    if (
                        seat[0][0] < 0 ||
                        seat[0][0] >= filmsStruct->list[film_id].rows ||
                        seat[0][1] < 0 ||
                        seat[0][1] >= filmsStruct->list[film_id].columns
                    ){
                        fprintf(stderr, "loadBookingsFromFile: Invalid seat\n");
                        fclose(file);
                        return i;
                    }
                break;

                case 4:
                    // example Q9LP15AJ-FD8OLH3Y
                    if (strcspn(token, "\n")){
                        token[strcspn(token, "\n")] = 0;
                    }
                    
                    if (strlen(token) != 17 || strcspn(token, "-") != 8){
                        fprintf(stderr, "loadBookingsFromFile: Invalid booking code\n");
                        fclose(file);
                        return i;
                    }

                    strncpy(booking_code[0], token, sizeof(booking_code));
                break;

                default:
                break;
            }

            token = strtok_r(NULL, ".", &saveptr);
            i++;
        }

        if (i == 5){
            //DEBUG
            // printf("film_id: %d\n", film_id);
            // printf("showtime_id: %d\n", showtime_id);
            // printf("seat: %c%d\n", seat[0][0] + 'A', seat[0][1] + 1);
            // printf("booking_code: %s\n", booking_code[0]);

            if (bookSeats(&filmsStruct->list[film_id].halls[showtime_id], 1, seat, booking_code) != 0){
                fprintf(stderr, "loadBookingsFromFile: Error booking seat\n");
                fclose(file);
                return 5;
            }
        }else{
            fprintf(stderr, "loadBookingsFromFile: Invalid line\n");
            fclose(file);
            return 6;
        }
    }

    fclose(file);
    return 0;
}


void printTicketToBuff(char **buff, char *bookingCode, char *filmTitle, char *filmShowtime, char *seat, size_t *remaining_size){
    appendToBuffer(buff, remaining_size, "============== BIGLIETTO =============\n");
    appendToBuffer(buff, remaining_size, "Codice prenotazione: %s\n", bookingCode);
    appendToBuffer(buff, remaining_size, "Film: %s\n", filmTitle);
    appendToBuffer(buff, remaining_size, "Orario: %s\n", filmShowtime);
    appendToBuffer(buff, remaining_size, "Posto: %s\n", seat);
    appendToBuffer(buff, remaining_size, "======================================\n\n\n");
}