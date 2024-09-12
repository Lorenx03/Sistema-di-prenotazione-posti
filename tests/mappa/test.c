#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROWS 8  // Dalla A alla G con un corridoio (fila vuota)
#define SEATS 9 // 9 posti per fila
#define DISABLED_SEATS 5
#define DISABLED_ROW 4 // Indice 4 corrisponde alla fila E
#define MAX_ORARI 10

typedef struct {
    char row;
    int seat_number;
    int is_booked;
    int is_disabled;
    int booking_code; // Codice unico per la prenotazione
} Seat;

typedef struct {
    Seat seats[ROWS][SEATS];
    int available_seats; // Numero di posti disponibili
} Cinema_Hall;

typedef struct {
    short id;
    char titolo[50];
    char genere[30];
    char lingua[20];
    int durata; // Durata in minuti
    char cast[200];
    char trama[500];
    char orari[100];
    int available_seats;
    Cinema_Hall **seats; // Un array di puntatori a sale cinematografiche
} Film;

// Funzione per creare una nuova sala cinematografica
Cinema_Hall* creaSala(int available_seats) {
    Cinema_Hall *sala = (Cinema_Hall*)malloc(sizeof(Cinema_Hall));
    sala->available_seats = available_seats;

    // Inizializzazione dei posti
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < SEATS; j++) {
            sala->seats[i][j].row = 'A' + i;
            sala->seats[i][j].seat_number = j + 1;
            sala->seats[i][j].is_booked = 0;
            sala->seats[i][j].booking_code = 0;
            sala->seats[i][j].is_disabled = (i == DISABLED_ROW && j < DISABLED_SEATS) ? 1 : 0;
        }
    }

    return sala;
}

// Funzione per leggere i film da un file CSV e creare i film in memoria
Film* creaFilmDaCSV(const char *filename, int *numFilms) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Errore nell'aprire il file.\n");
        return NULL;
    }

    char buffer[1000];
    *numFilms = 0;

    // Conta il numero di film (salta la prima riga di intestazione)
    while (fgets(buffer, sizeof(buffer), file)) {
        if (*numFilms > 0) (*numFilms)++;
    }

    rewind(file); // Torna all'inizio del file
    fgets(buffer, sizeof(buffer), file); // Salta l'intestazione

    // Alloca memoria per i film
    Film *films = (Film*)malloc((*numFilms) * sizeof(Film));

    int filmIndex = 0;
    while (fgets(buffer, sizeof(buffer), file)) {
        char *token = strtok(buffer, ",");

        // Titolo
        strncpy(films[filmIndex].titolo, token, sizeof(films[filmIndex].titolo) - 1);
        token = strtok(NULL, ",");

        // Genere
        strncpy(films[filmIndex].genere, token, sizeof(films[filmIndex].genere) - 1);
        token = strtok(NULL, ",");

        // Lingua
        strncpy(films[filmIndex].lingua, token, sizeof(films[filmIndex].lingua) - 1);
        token = strtok(NULL, ",");

        // Durata
        films[filmIndex].durata = atoi(token);
        token = strtok(NULL, ",");

        // Cast
        strncpy(films[filmIndex].cast, token, sizeof(films[filmIndex].cast) - 1);
        token = strtok(NULL, ",");

        // Trama
        strncpy(films[filmIndex].trama, token, sizeof(films[filmIndex].trama) - 1);
        token = strtok(NULL, "\"");

        // Orari
        strncpy(films[filmIndex].orari, token, sizeof(films[filmIndex].orari) - 1);
        token = strtok(NULL, ",");

        // Posti disponibili
        films[filmIndex].available_seats = atoi(token);

        // Conta il numero di orari
        int numOrari = 0;
        char *orario = strtok(films[filmIndex].orari, ";");
        while (orario != NULL) {
            numOrari++;
            orario = strtok(NULL, ";");
        }

        // Alloca memoria per le sale in base al numero di orari
        films[filmIndex].seats = (Cinema_Hall**)malloc(numOrari * sizeof(Cinema_Hall*));

        // Crea una sala per ogni orario
        for (int j = 0; j < numOrari; j++) {
            films[filmIndex].seats[j] = creaSala(films[filmIndex].available_seats);
        }

        filmIndex++;
    }

    fclose(file);
    return films;
}

// Funzione per liberare la memoria allocata
void liberaFilm(Film *films, int numFilms) {
    for (int i = 0; i < numFilms; i++) {
        char *orario = strtok(films[i].orari, ";");
        int numOrari = 0;
        while (orario != NULL) {
            free(films[i].seats[numOrari]);
            orario = strtok(NULL, ";");
            numOrari++;
        }
        free(films[i].seats);
    }
    free(films);
}

int main() {
    int numFilms;
    Film *films = creaFilmDaCSV("film.csv", &numFilms);

    if (films == NULL) {
        return 1;
    }

    // Esempio di utilizzo dei dati caricati
    for (int i = 0; i < numFilms; i++) {
        printf("Film: %s\n", films[i].titolo);
        printf("Orari: %s\n", films[i].orari);
    }

    // Libera la memoria allocata
    liberaFilm(films, numFilms);

    return 0;
}
