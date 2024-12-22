#include "cinema.h"
#include "filmsCSVparser.h"

Films *initFilmsList(const char *filename) {
    Films *filmsStruct = (Films *)malloc(sizeof(Films));
    if (filmsStruct == NULL){
        fprintf(stderr, "Error: could not allocate memory for films\n");
        exit(1);
    }

    read_films_csv(filename, &filmsStruct->list, &filmsStruct->count); // Read the CSV file and store the films

    // Inizializza le sale per ogni film
    for (int i = 0; i < filmsStruct->count; i++) {
        filmsStruct->list[i].numbers_showtimes = count_showtimes(&filmsStruct->list[i]);
        // create_halls_for_showtimes(&filmsStruct->list[i]);
    }

    return filmsStruct;
}