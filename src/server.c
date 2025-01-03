#include <stdio.h>
#include "httpServer.h"
#include "cinema.h"
#include "filmsCSVparser.h"

// Global context
Films cinemaFilms = {0};

// DEBUG
// void GETrootHandler(char *request, char *response) {
//     char response_body[100];
//     snprintf(response_body, sizeof(response_body), "Root - Thread ID: %ld", (long)pthread_self());
//     httpResponseBuilder(response, 200, "OK", response_body);
// }

void GETrootHandler(char *request, char *response) {
    char response_body[MAX_RESPONSE_SIZE] = {0};

    snprintf(response_body, sizeof(response_body),
             "Benvenuto al Cinema\n\n"
             "Menù\n\n"
             "1. Programmazione sale\n"
             "2. Acquista biglietto\n"
             "3. Disdire una prenotazione\n"
             "4. Esci\n");

    httpResponseBuilder(response, 200, "OK", response_body);
}

void GETfilmsHandler(char *request, char *response) {
    char response_body[MAX_RESPONSE_SIZE] = {0};
    snprintf(response_body, sizeof(response_body), "Lista dei film:\n");
    print_films(response_body, MAX_RESPONSE_SIZE, cinemaFilms.list, cinemaFilms.count);
    httpResponseBuilder(response, 200, "OK", response_body);
}

void GETFilmsListHandler(char *request, char *response) {
    char response_body[MAX_RESPONSE_SIZE] = {0};
    snprintf(response_body, sizeof(response_body), "Lista dei film:\n");
    print_films_name(response_body, MAX_RESPONSE_SIZE, cinemaFilms.list, cinemaFilms.count);
    httpResponseBuilder(response, 200, "OK", response_body);
}

void GETBookShowtimesListHandler(char *request, char *response) {
    char response_body[MAX_RESPONSE_SIZE] = {0};

    bool success = false;
    int selected_film = 0;
    errno = 0;
    char *endptr;

    selected_film = strtol(request, &endptr,10);

    if(errno == ERANGE){
        success = false;
    }else if(endptr == request){
        success = false;
    }else if(*endptr && *endptr != '\n'){
        success = false;
    }else{
        success = true;
    }

    if (success && selected_film > 0 && selected_film <= cinemaFilms.count){
        Film *film = &cinemaFilms.list[selected_film - 1];
        char listOfShowTimes[1000] = {0};

        int showtimeIndex = 1;
        char *saveptr;
        char *showtime = strtok_r(film->showtimes, ",", &saveptr);

        while (showtime != NULL) {
            snprintf(listOfShowTimes + strlen(listOfShowTimes), sizeof(listOfShowTimes) - strlen(listOfShowTimes), "%d. %s\n", showtimeIndex, showtime);
            showtimeIndex++;
            showtime = strtok_r(NULL, ",", &saveptr);
        }
        
        snprintf(response_body, sizeof(response_body), "Lista degli orari disponibili per %s:\n%s", film->name, listOfShowTimes);
    }else{
        snprintf(response_body, sizeof(response_body), "Film non trovato\n");
    }
    
    httpResponseBuilder(response, 200, "OK", response_body);
}


int main() {
    //Random seed for booking code generation
    srand(time(NULL));
    
    HttpRoute rootRoute = {0};
    rootRoute.name = "/";
    rootRoute.handlers[GET] = GETrootHandler;

    HttpRoute filmsRoute = {0};
    filmsRoute.name = "films";
    filmsRoute.handlers[GET] = GETfilmsHandler;

    HttpRoute filmsListRoute = {0};
    filmsListRoute.name = "list";
    filmsListRoute.handlers[GET] = GETFilmsListHandler;

    HttpRoute bookShowtimesListRoute = {0};
    bookShowtimesListRoute.name = "book";
    bookShowtimesListRoute.handlers[GET] = GETBookShowtimesListHandler;


    addHttpSubroute(&rootRoute, &filmsRoute);
    addHttpSubroute(&rootRoute, &bookShowtimesListRoute);
    addHttpSubroute(&filmsRoute, &filmsListRoute);

    HttpServer server = {
        .port = 8090,
        .numThreads = 1,
        .root = &rootRoute
    };

    initFilmsList("films.csv", &cinemaFilms);

    if (httpServerServe(&server)) {
        fprintf(stderr, "Error starting server\n");
        return 1;
    }
    return 0;
}