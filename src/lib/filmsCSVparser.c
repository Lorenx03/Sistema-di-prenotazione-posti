#include "cinema.h"
#include "filmsCSVparser.h"

// Function to remove leading and trailing whitespace
char* trim_whitespace(char* str) {
    char* end;

    // Trim leading space
    while (isspace((unsigned char)*str)) str++;

    // Trim trailing space
    if (*str == 0)  // All spaces?
        return str;

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator
    *(end + 1) = '\0';

    return str;
}

// Function to parse a CSV line, handling quoted fields with commas
void parse_films_csv_line(char *line, Film *film) {
    //char *token;
    char *field;
    int field_count = 0;
    bool inside_quotes = false;
    char buffer[4096];  // Buffer to store concatenated tokens
    int buffer_index = 0;

    for (int i = 0; line[i] != '\0'; i++) {
        char c = line[i];

        if (c == '"') {
            // Toggle inside_quotes flag when encountering a quote
            inside_quotes = !inside_quotes;
        } else if (c == ',' && !inside_quotes) {
            // If we encounter a comma and are not inside quotes, process the field
            buffer[buffer_index] = '\0';  // Null-terminate the buffer
            field = strdup(trim_whitespace(buffer));  // Trim and copy the field

            // Assign the field to the appropriate Film structure field
            switch (field_count) {
                case 0: film->name = field; break;
                case 1: film->genre = field; break;
                case 2: film->language = field; break;
                case 3: film->duration = atoi(field); free(field); break;
                case 4: film->actors = field; break;
                case 5: film->plot = field; break;
                case 6: film->showtimes = field; break;
                case 7: film->rows = atoi(field); free(field); break;
                case 8: film->columns = atoi(field); free(field); break;
            }

            buffer_index = 0;  // Reset buffer for next field
            field_count++;
        } else {
            // Append the current character to the buffer
            buffer[buffer_index++] = c;
        }
    }

    film -> numbers_showtimes = count_showtimes(film);


    
    // Handle the last field (since there's no trailing comma)
    buffer[buffer_index] = '\0';  // Null-terminate the buffer
    field = strdup(trim_whitespace(buffer));  // Trim and copy the field
    if (field_count == 8) {
        film->columns = atoi(field);
        free(field);
    }
}

// Function to calculate the total seats in the theater
int calculate_total_seats(Film *film) {
    return film->rows * film->columns;
}

// Function to count the number of showtimes
int count_showtimes(Film *film) {
    int count = 0;
    char *showtimes = strdup(film->showtimes);  // Copy to avoid modifying the original
    char *token = strtok(showtimes, ",");

    while (token) {
        count++;
        token = strtok(NULL, ",");
    }

    free(showtimes);
    return count;
}

// Function to dynamically read the CSV file and store the data into the Film array
void read_films_csv(const char *filename, Film **films, int *film_count) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: could not open file %s\n", filename);
        exit(1);
    }

    char line[4096];
    int count = 0;
    int capacity = 10;  // Initial capacity for the array of films

    // Allocate memory for films dynamically
    *films = (Film *)malloc(capacity * sizeof(Film));

    while (fgets(line, sizeof(line), file)) {
        if (count >= capacity) {
            // Double the capacity if needed
            capacity *= 2;
            *films = (Film *)realloc(*films, capacity * sizeof(Film));
        }

        // Parse each line and fill the film structure
        parse_films_csv_line(line, &(*films)[count]);

        count++;
    }

    *film_count = count;
    fclose(file);
}

// Function to free memory for films
void free_films(Film *films, int film_count) {
    for (int i = 0; i < film_count; i++) {
        free(films[i].name);
        free(films[i].genre);
        free(films[i].language);
        free(films[i].actors);
        free(films[i].plot);
        free(films[i].showtimes);
    }
    free(films);
}

// Function to print film details (for testing)
void print_films(char *buffer, Film *films, int film_count) {
    for (int i = 0; i < film_count; i++) {
        sprintf(buffer + strlen(buffer), "Film %d:\n", i + 1);
        sprintf(buffer + strlen(buffer), "  Nome: %s\n", films[i].name);
        sprintf(buffer + strlen(buffer), "  Genere: %s\n", films[i].genre);
        sprintf(buffer + strlen(buffer), "  Lingua: %s\n", films[i].language);
        sprintf(buffer + strlen(buffer), "  Durata: %d minuti\n", films[i].duration);
        sprintf(buffer + strlen(buffer), "  Attori: %s\n", films[i].actors);
        sprintf(buffer + strlen(buffer), "  Trama: %s\n", films[i].plot);
        sprintf(buffer + strlen(buffer), "  Orari: %s\n", films[i].showtimes);
        sprintf(buffer + strlen(buffer), "  Posti totali: %d\n", calculate_total_seats(&films[i]));
        sprintf(buffer + strlen(buffer), "\n");
    }
}

// Funzione per stampare i nomi dei film
void print_films_name(Film *films, int film_count) {
    for (int i = 0; i < film_count; i++) {
        printf("Film %d: %s\n", i + 1,films[i].name);
    }
}

// Function to count the number of lines in the CSV file
int count_csv_lines(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: could not open file %s\n", filename);
        return -1; // Indicate error opening file
    }

    char line[4096];
    int line_count = 0;

    // Count each line in the file
    while (fgets(line, sizeof(line), file)) {
        line_count++;
    }

    fclose(file);
    return line_count;
}