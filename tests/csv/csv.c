#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Definizione della struttura Film
typedef struct {
    short id;
    char titolo[50];
    char genere[30];
    char lingua[20];
    int durata; // Durata in minuti
    char cast[200];
    char trama[500];
    char orari[100];
    int available_seats; // Numero di posti disponibili
} Film;


#define MAX_LINE_LENGTH 1000

Film *parse_csv_film(const char* filename, int* num_films) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Errore nell'apertura del file");
        return NULL;
    }

    Film* films = NULL;
    int capacity = 10;
    int count = 0;
    char line[MAX_LINE_LENGTH];

    films = malloc(capacity * sizeof(Film));
    if (!films) {
        fclose(file);
        return NULL;
    }

    while (fgets(line, sizeof(line), file)) {
        if (count >= capacity) {
            capacity *= 2;
            Film* temp = realloc(films, capacity * sizeof(Film));
            if (!temp) {
                free(films);
                fclose(file);
                return NULL;
            }
            films = temp;
        }

        Film film = {0};
        film.id = count + 1;  // Assegna un ID univoco

        char* token;
        char* rest = line;
        int field = 0;
        bool in_quotes = false;
        char buffer[MAX_LINE_LENGTH] = {0};
        int buffer_pos = 0;

        while ((token = strtok_r(rest, ",", &rest))) {
            for (int i = 0; token[i]; i++) {
                if (token[i] == '"') {
                    in_quotes = !in_quotes;
                } else {
                    if (buffer_pos < sizeof(buffer) - 1) {
                        buffer[buffer_pos++] = token[i];
                    }
                }
            }

            if (!in_quotes) {
                buffer[buffer_pos] = '\0';
                switch (field) {
                    case 0: strncpy(film.titolo, buffer, sizeof(film.titolo) - 1); break;
                    case 1: strncpy(film.genere, buffer, sizeof(film.genere) - 1); break;
                    case 2: strncpy(film.lingua, buffer, sizeof(film.lingua) - 1); break;
                    case 3: film.durata = atoi(buffer); break;
                    case 4: strncpy(film.cast, buffer, sizeof(film.cast) - 1); break;
                    case 5: strncpy(film.trama, buffer, sizeof(film.trama) - 1); break;
                    case 6: strncpy(film.orari, buffer, sizeof(film.orari) - 1); break;
                }
                field++;
                buffer_pos = 0;
            } else {
                buffer[buffer_pos++] = ',';
            }
        }

        films[count++] = film;
    }

    fclose(file);
    *num_films = count;
    return films;
}


int main() {
    int num_films;
    Film* films = parse_csv_film("film_data.csv", &num_films);
    if (!films) {
        return 1;
    }

    for (int i = 0; i < num_films; i++) {
        printf("Film %d\n", films[i].id);
        printf("Titolo: %s\n", films[i].titolo);
        printf("Genere: %s\n", films[i].genere);
        printf("Lingua: %s\n", films[i].lingua);
        printf("Durata: %d minuti\n", films[i].durata);
        printf("Cast: %s\n", films[i].cast);
        printf("Trama: %s\n", films[i].trama);
        printf("Orari: %s\n", films[i].orari);
        printf("\n");
    }

    free(films);
    return 0;
}