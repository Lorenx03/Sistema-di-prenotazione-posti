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
    char *current_ptr = response_body;  // Add this line

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

    httpResponseBuilder(response, 200, "OK", response_body);
}

// Http route: /films
void GETFilmsHandler(char *request, char *response) {
    (void)request;
    char response_body[MAX_RESPONSE_SIZE] = {0};
    snprintf(response_body, sizeof(response_body), COLOR_BOLD"Lista dei film:\n"COLOR_OFF);
    print_films(response_body, MAX_RESPONSE_SIZE, cinemaFilms.list, cinemaFilms.count);
    httpResponseBuilder(response, 200, "OK", response_body);
}

// Http route: /films/list
void GETFilmsListHandler(char *request, char *response) {
    (void)request;
    char response_body[MAX_RESPONSE_SIZE] = {0};
    snprintf(response_body, sizeof(response_body), "Lista dei film:\n");
    print_films_name(response_body, MAX_RESPONSE_SIZE, cinemaFilms.list, cinemaFilms.count);
    httpResponseBuilder(response, 200, "OK", response_body);
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

        httpResponseBuilder(response, 200, "OK", response_body);
    }else{
        snprintf(response_body, sizeof(response_body), "Film non trovato\n");
        httpResponseBuilder(response, 404, "Not Found", response_body);
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


    addHttpSubroute(&rootRoute, &filmsRoute);
    addHttpSubroute(&rootRoute, &bookShowtimesListRoute);
    addHttpSubroute(&filmsRoute, &filmsListRoute);
    addHttpSubroute(&filmsRoute, &filmHallMapRoute);

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