// Include local headers
#include "cinema.h"
#include "filmsCSVparser.h"

// Include platform-specific system headers
#ifdef __APPLE__
#include <sys/event.h>  // For macOS/FreeBSD
#elif defined(__linux__)
#include <sys/epoll.h>  // For Linux
#else
#error "Unsupported operating system"
#endif

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
    char showtimes[1024] = {0};
    
    snprintf(showtimes, sizeof(showtimes), "%s", film->showtimes);
    char *token = strtok(showtimes, ",");

    while (token) {
        count++;
        token = strtok(NULL, ",");
    }

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

// Function to print film details to a buffer (for testing)
void print_films(char *buffer, size_t buffer_size, Film *films, int film_count) {
    size_t offset = strlen(buffer);
    for (int i = 0; i < film_count; i++) {
        size_t written = snprintf(buffer + offset, buffer_size - offset,
            "Film %d:\n"
            "  Titolo: %s\n"
            "  Genere: %s\n"
            "  Lingua: %s\n"
            "  Durata: %d minuti\n"
            "  Attori: %s\n"
            "  Trama: %s\n"
            "  Orari: %s\n"
            "  Posti totali: %d\n\n",
            i + 1,
            films[i].name,
            films[i].genre,
            films[i].language,
            films[i].duration,
            films[i].actors,
            films[i].plot,
            films[i].showtimes,
            calculate_total_seats(&films[i])
        );

        if (written >= buffer_size - offset) {
            fprintf(stderr, "Error writing to buffer\n");
            break;
        }

        offset += written;
    }
}

// Funzione per stampare i nomi dei film
void print_films_name(char *buffer, size_t buffer_size, Film *films, int film_count) {
    size_t offset = strlen(buffer);
    for (int i = 0; i < film_count; i++) {
        size_t written = snprintf(buffer + offset, buffer_size - offset, "Film %d: %s\n", i + 1, films[i].name);
        if (written >= buffer_size - offset) {
            fprintf(stderr, "Error writing to buffer\n");
            break;
        }
        offset += written;
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