#include <stdio.h>
#include "httpServer.h"
#include "cinema.h"
#include "filmsCSVparser.h"

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
    Films *films = initFilmsList("films.csv");
    print_films(response_body, MAX_RESPONSE_SIZE, films->list, films->count);
    httpResponseBuilder(response, 200, "OK", response_body);
}


int main() {
    //Random seed for booking code generation
    srand(time(NULL));
    
    HttpRoute root = {0};
    root.name = "/";
    root.handlers[GET] = GETrootHandler;

    HttpRoute films = {0};
    films.name = "films";
    films.handlers[GET] = GETfilmsHandler;

    addHttpSubroute(&root, &films);

    HttpServer server = {
        .port = 8090,
        .numThreads = 1,
        .root = &root
    };

    if (httpServerServe(&server)) {
        fprintf(stderr, "Error starting server\n");
        return 1;
    }
    return 0;
}