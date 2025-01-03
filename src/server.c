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

    addHttpSubroute(&rootRoute, &filmsRoute);
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