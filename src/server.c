#include <stdio.h>
#include "httpServer.h"
#include "cinema.h"
#include "filmsCSVparser.h"
#include "utils.h"

#define COLOR_BOLD  "\e[1m"
#define COLOR_OFF   "\e[m"

// Global context
Films cinemaFilms = {0};
// DEBUG
// void GETrootHandler(char *request, char *response) {
//     char response_body[100];
//     snprintf(response_body, sizeof(response_body), "Root - Thread ID: %ld", (long)pthread_self());
//     httpResponseBuilder(response, 200, "OK", response_body);
// }

// Http route: /
void GETrootHandler(char *request, char *response) {
    (void)request;
    char response_body[MAX_RESPONSE_SIZE] = {0};
    size_t buffSize = MAX_RESPONSE_SIZE;
    char *current_ptr = response_body;

    appendToBuffer(&current_ptr, &buffSize,"===================================================================\n");

    appendToBuffer(&current_ptr, &buffSize,
             "\n"
             " ________  ___  ________   _______   _____ ______   ________     \n"
             "|\\   ____\\|\\  \\|\\   ___  \\|\\  ___ \\ |\\   _ \\  _   \\|\\   __  \\    \n"
             "\\ \\  \\___|\\ \\  \\ \\  \\\\ \\  \\ \\   __/|\\ \\  \\\\\\__\\ \\  \\ \\  \\|\\  \\   \n"
             " \\ \\  \\    \\ \\  \\ \\  \\\\ \\  \\ \\  \\_|/_\\ \\  \\\\|__| \\  \\ \\   __  \\  \n"
             "  \\ \\  \\____\\ \\  \\ \\  \\\\ \\  \\ \\  \\_|\\ \\ \\  \\    \\ \\  \\ \\  \\ \\  \\ \n"
             "   \\ \\_______\\ \\__\\ \\__\\\\ \\__\\ \\_______\\ \\__\\    \\ \\__\\ \\__\\ \\__\\\n"
             "    \\|_______|\\|__|\\|__| \\|__|\\|_______|\\|__|     \\|__|\\|__|\\|__|\n"
             "\n\n");

    appendToBuffer(&current_ptr, &buffSize,"===================================================================\n\n");

    appendToBuffer(&current_ptr, &buffSize,
             "1. Programmazione sale\n"
             "2. Acquista biglietto\n"
             "3. Disdire una prenotazione\n"
             "4. Esci\n");

    httpResponseBuilder(response, HTTP_STATUS_OK, "OK", response_body);
}

// Http route: /films
void GETFilmsHandler(char *request, char *response) {
    (void)request;
    char response_body[MAX_RESPONSE_SIZE] = {0};
    snprintf(response_body, sizeof(response_body), COLOR_BOLD"Lista dei film:\n"COLOR_OFF);
    print_films(response_body, MAX_RESPONSE_SIZE, cinemaFilms.list, cinemaFilms.count);
    httpResponseBuilder(response, HTTP_STATUS_OK, "OK", response_body);
}

// Http route: /films/list
void GETFilmsListHandler(char *request, char *response) {
    (void)request;
    char response_body[MAX_RESPONSE_SIZE] = {0};
    snprintf(response_body, sizeof(response_body), "Lista dei film:\n");
    print_films_name(response_body, MAX_RESPONSE_SIZE, cinemaFilms.list, cinemaFilms.count);
    httpResponseBuilder(response, HTTP_STATUS_OK, "OK", response_body);
}

// Http route: /films/showtimes
void GETBookShowtimesListHandler(char *request, char *response) {
    char response_body[MAX_RESPONSE_SIZE] = {0};
    int selected_film = safeStrToInt(request);

    if (selected_film > 0 && selected_film <= cinemaFilms.count){
        Film *film = &cinemaFilms.list[selected_film - 1];
        char listOfShowTimes[1000] = {0};

        int showtimeIndex = 1;
        char *saveptr;
        char *temp = strdup(film->showtimes);
        char *showtime = strtok_r(temp, ",", &saveptr);

        while (showtime != NULL) {
            snprintf(listOfShowTimes + strlen(listOfShowTimes), sizeof(listOfShowTimes) - strlen(listOfShowTimes), "%d. "COLOR_BOLD"%s"COLOR_OFF"\n", showtimeIndex, showtime);
            showtimeIndex++;
            showtime = strtok_r(NULL, ",", &saveptr);
        }
        
        snprintf(response_body, sizeof(response_body), "Lista degli orari disponibili per: "COLOR_BOLD"%s"COLOR_OFF"\n%s", film->name, listOfShowTimes);
        free(temp);

        httpResponseBuilder(response, HTTP_STATUS_OK, "OK", response_body);
    }else{
        snprintf(response_body, sizeof(response_body), "Film non trovato\n");
        httpResponseBuilder(response, HTTP_STATUS_NOT_FOUND, "Not Found", response_body);
    }
}

// Http route: /films/map
void GETFilmHallMapHandler(char *request, char *response) {
    char response_body[MAX_RESPONSE_SIZE] = {0};

    //exaple request: 1.1
    char *saveptr;
    char *token = strtok_r(request, ".", &saveptr);
    int selected_film = -1, hall_index = -1;

    if (token != NULL) {
        selected_film = safeStrToInt(token);
        token = strtok_r(NULL, ".", &saveptr);
        if (token != NULL) {
            hall_index = safeStrToInt(token);
        }
    }

    printf("selected_film: %d\n", selected_film);
    printf("hall_index: %d\n", hall_index);

    if (selected_film > 0 && selected_film <= cinemaFilms.count && hall_index > 0 && hall_index <= cinemaFilms.list[selected_film - 1].numbers_showtimes){
        Film *film = &cinemaFilms.list[selected_film - 1];
        generateHallMapResponse(&film->halls[hall_index - 1], response_body, sizeof(response_body));
    }else{
        snprintf(response_body, sizeof(response_body), "Film non trovato\n");
    }
    
    printf("response_body: %s\n", response_body);
    httpResponseBuilder(response, HTTP_STATUS_OK, "OK", response_body);
}

// Http route: /book
void POSTBookSeat(char *request, char *response){
    char response_body[MAX_RESPONSE_SIZE] = {0};
    size_t buffSize = MAX_RESPONSE_SIZE;
    char *current_ptr = response_body;

    //exaple request: 1.1
    char *saveptr;
    char *token = strtok_r(request, ".", &saveptr);
    int selected_film = -1, hall_index = -1;
    char showtime[6] = {0};

    // Prenotation code -> A unqiue code for the prenotation, a prenotation can have multiple bookings
    char prenotationCode[9] = {0};
    generateRandomString(prenotationCode, sizeof(prenotationCode)-1);

    // reservationIdentifier -> A unique code for the booking
    char bookingCode[9] = {0};
    char reservationIdentifier[9] = {0};
    generateRandomString(bookingCode, sizeof(bookingCode)-1);
    char bookingCodes[4][18] = {0};


    int seatsToBook[4][2];
    for (int i = 0; i < 4; i++){
        seatsToBook[i][0] = -1;
        seatsToBook[i][1] = -1;
    }


    if (token != NULL) { // 1st token is the film index
        selected_film = safeStrToInt(token);
        token = strtok_r(NULL, ".", &saveptr);

        if (selected_film < 0 || selected_film > cinemaFilms.count){
            httpResponseBuilder(response, HTTP_STATUS_BAD_REQUEST, "Bad Request", "Richiesta non valida\n");
            return;
        }

        if (token != NULL) { // 2nd token is the hall index
            hall_index = safeStrToInt(token);

            if (hall_index < 0 || hall_index > cinemaFilms.list[selected_film - 1].numbers_showtimes){
                httpResponseBuilder(response, HTTP_STATUS_BAD_REQUEST, "Bad Request", "Richiesta non valida\n");
                return;
            }

            printf("selected_film: %d\n", selected_film);
            printf("hall_index: %d\n", hall_index);

            // Select the film and the hall
            Film *film = &cinemaFilms.list[selected_film - 1];
            // Showtime for the ticket
            getNthToken(film->showtimes, ",", hall_index - 1, showtime, sizeof(showtime));

            // Append the prenotation code to the response, for filename of the tickets
            appendToBuffer(&current_ptr, &buffSize, "%s\n", prenotationCode);
            
            token = strtok_r(NULL, ".", &saveptr); // 3rd, 4th, 5th, 6th token are the seats to book
            int numSeats = 0;
            while (token != NULL && numSeats < 4) {
                if(strlen(token) < 2 || strlen(token) > 3){
                    httpResponseBuilder(response, HTTP_STATUS_BAD_REQUEST, "Bad Request", "Richiesta non valida\n");
                    return;
                }

                int row = token[0] - 'A';
                int column = safeStrToInt(&token[1]) - 1; // -2 if error
                
                if (row < 0 || row >= film->rows || column < 0 || column >= film->columns){
                    httpResponseBuilder(response, HTTP_STATUS_BAD_REQUEST, "Bad Request", "Richiesta non valida\n");
                    return;
                }

                seatsToBook[numSeats][0] = row;
                seatsToBook[numSeats][1] = column;

                generateRandomString(reservationIdentifier, sizeof(reservationIdentifier)-1);
                snprintf(bookingCodes[numSeats], sizeof(bookingCodes[numSeats]), "%s-%s", bookingCode, reservationIdentifier);

                token = strtok_r(NULL, ".", &saveptr);
                numSeats++;
            }

            if (numSeats == 0){
                httpResponseBuilder(response, HTTP_STATUS_BAD_REQUEST, "Bad Request", "Richiesta non valida\n");
                return;
            }

            if (bookSeats(&film->halls[hall_index - 1], numSeats, seatsToBook, bookingCodes) == 0){
                for (int i = 0; i < numSeats; i++){
                    printTicketToBuff(&current_ptr, bookingCodes[i], film->name, showtime, request, &buffSize);
                }
                httpResponseBuilder(response, HTTP_STATUS_CREATED, "OK", response_body);
            }else{
                httpResponseBuilder(response, HTTP_STATUS_INTERNAL_SERVER_ERROR, "Internal Server Error", "Errore interno del server\n");
            }
        }
    }
}

// Http route: /unbook
void POSTUnbookSeat(char *request, char *response) {
    if (strlen(request) == 0) {
        httpResponseBuilder(response, HTTP_STATUS_BAD_REQUEST, "Bad Request", "Richiesta non valida\n");
        return;
    }
    bool found = false;

    printf("POSTUnbookSeat - %s\n", request);

    for(int i = 0; i < cinemaFilms.count; i++){
        for(int j = 0; j < cinemaFilms.list[i].numbers_showtimes; j++){
            for(int k = 0; k < cinemaFilms.list[i].halls[j].rows; k++){
                for(int l = 0; l < cinemaFilms.list[i].halls[j].columns; l++){
                    if (
                        cinemaFilms.list[i].halls[j].seats[k][l].state == BOOKED &&
                        strlen(cinemaFilms.list[i].halls[j].seats[k][l].booking_code) == 17 &&
                        (   
                            (
                                strlen(request) == 8 &&
                                strncmp(cinemaFilms.list[i].halls[j].seats[k][l].booking_code, request, 8) == 0
                            ) 
                            ||
                            (
                                strlen(request) == 17 &&
                                strncmp(cinemaFilms.list[i].halls[j].seats[k][l].booking_code, request, 17) == 0
                            )
                        )
                    ){
                        if(pthread_mutex_trylock(&cinemaFilms.list[i].halls[j].seats[k][l].lock) == 0){
                            found = true;
                            
                            printf("Removing prenotation: %s\n", cinemaFilms.list[i].halls[j].seats[k][l].booking_code);
                            cinemaFilms.list[i].halls[j].seats[k][l].state = FREE;
                            memset(cinemaFilms.list[i].halls[j].seats[k][l].booking_code, 0, sizeof(cinemaFilms.list[i].halls[j].seats[k][l].booking_code));

                            pthread_mutex_unlock(&cinemaFilms.list[i].halls[j].seats[k][l].lock);
                        }
                    }
                }
            }
        }
    }

    if (found) {
        httpResponseBuilder(response, HTTP_STATUS_OK, "OK", "Prenoazione cancellata con successo.\n");
    }else{
        httpResponseBuilder(response, HTTP_STATUS_NOT_FOUND, "Not Found", "Prenoazione non trovata.\n");
    }
}



// Cron job that saves the current state of the data structures to a file, so that the threads dont have to do it
void saveBookingsCronJob(void) {
    printf("Saving bookings to file\n");
    saveBookingsToFile(&cinemaFilms, "bookings.csv");
}


int main() {
    //Random seed for booking code generation
    srand(time(NULL));
    
    HttpRoute rootRoute = {0};
    rootRoute.name = "/";
    rootRoute.handlers[GET] = GETrootHandler;

    HttpRoute filmsRoute = {0};
    filmsRoute.name = "films";
    filmsRoute.handlers[GET] = GETFilmsHandler;

    HttpRoute filmsListRoute = {0};
    filmsListRoute.name = "list";
    filmsListRoute.handlers[GET] = GETFilmsListHandler;

    HttpRoute filmHallMapRoute = {0};
    filmHallMapRoute.name = "map";
    filmHallMapRoute.handlers[GET] = GETFilmHallMapHandler;

    HttpRoute bookShowtimesListRoute = {0};
    bookShowtimesListRoute.name = "showtimes";
    bookShowtimesListRoute.handlers[GET] = GETBookShowtimesListHandler;

    HttpRoute bookRoute = {0};
    bookRoute.name = "book";
    bookRoute.handlers[POST] = POSTBookSeat;

    HttpRoute bookUnbookRoute = {0};
    bookUnbookRoute.name = "unbook";
    bookUnbookRoute.handlers[POST] = POSTUnbookSeat;


    addHttpSubroute(&rootRoute, &filmsRoute); // /films
    addHttpSubroute(&rootRoute, &bookRoute); // /book
    addHttpSubroute(&rootRoute, &bookUnbookRoute); // /unbook

    addHttpSubroute(&filmsRoute, &bookShowtimesListRoute); // /films/showtimes
    addHttpSubroute(&filmsRoute, &filmsListRoute); // /films/list
    addHttpSubroute(&filmsRoute, &filmHallMapRoute); // /films/map

    CronJob jobs[1] = {
        {
            .job = saveBookingsCronJob,
            .interval = 3
        }
    };
    
    HttpServerCronJobs cronJobs = {
        .jobs = jobs,
        .numJobs = 1
    };

    HttpServer server = {
        .port = 8090,
        .numThreads = 1,
        .cronJobs = &cronJobs,
        .root = &rootRoute
    };

    initFilmsList("films.csv", &cinemaFilms);
    loadBookingsFromFile(&cinemaFilms, "bookings.csv");

    if (httpServerServe(&server)) {
        fprintf(stderr, "Error starting server\n");
        return 1;
    }

    return 0;
}