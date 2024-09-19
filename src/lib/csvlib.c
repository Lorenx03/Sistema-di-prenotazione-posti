#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdarg.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 4096

typedef struct {
    char filename[1024];
    char **headings;
    size_t headings_count;
    char ***rows;
    size_t rows_count;
    FILE *file;
} CSVFile;



// ============================== READ ==============================
void readCsvHeadings(CSVFile *csv) {
    if (csv->file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];
    if (fgets(line, sizeof(line), csv->file) != NULL) {
        line[strcspn(line, "\n")] = '\0';
        char *saveptr;
        char *token = strtok_r(line, ",", &saveptr);
        
        while (token != NULL) {
            csv->headings = realloc(csv->headings, (csv->headings_count + 1) * sizeof(char*));
            if (csv->headings == NULL) {
                perror("Errore nell'allocazione della memoria per le intestazioni");
                exit(EXIT_FAILURE);
            }
            
            csv->headings[csv->headings_count] = strdup(token);
            if (csv->headings[csv->headings_count] == NULL) {
                perror("Errore nella duplicazione del token");
                exit(EXIT_FAILURE);
            }
            
            csv->headings_count++;
            token = strtok_r(NULL, ",", &saveptr);
        }
    }
}

void readCsvRows(CSVFile *csv) {
    if (csv->file == NULL) {
        perror("Errore nell'apertura del file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];
    csv->rows = NULL;
    csv->rows_count = 0;

    while (fgets(line, sizeof(line), csv->file) != NULL) {

        if (line[0] == '\n' || line[0] == '\0') {
            continue;
        }

        line[strcspn(line, "\n")] = '\0';
        csv->rows = realloc(csv->rows, (csv->rows_count + 1) * sizeof(char**));
        if (csv->rows == NULL) {
            perror("Errore nell'allocazione della memoria per le righe");
            exit(EXIT_FAILURE);
        }

        csv->rows[csv->rows_count] = NULL;
        size_t col_count = 0;
        char *saveptr;
        char *token = strtok_r(line, ",", &saveptr);
        
        while (token != NULL) {
            csv->rows[csv->rows_count] = realloc(csv->rows[csv->rows_count], (col_count + 1) * sizeof(char*));
            if (csv->rows[csv->rows_count] == NULL) {
                perror("Errore nell'allocazione della memoria per le colonne");
                exit(EXIT_FAILURE);
            }
            
            csv->rows[csv->rows_count][col_count] = strdup(token);
            if (csv->rows[csv->rows_count][col_count] == NULL) {
                perror("Errore nella duplicazione del token");
                exit(EXIT_FAILURE);
            }
            
            col_count++;
            token = strtok_r(NULL, ",", &saveptr);
        }

        csv->rows_count++;
    }
}








// ============================== INIT ==============================
CSVFile *csvInit(char *filename) {
    CSVFile *newCSVfile = malloc(sizeof(CSVFile));
    if (newCSVfile == NULL) {
        perror("Errore nell'allocazione della memoria per il file CSV");
        exit(EXIT_FAILURE);
    }

    strcpy(newCSVfile->filename, filename);
    if (newCSVfile->filename == NULL) {
        perror("Errore nella duplicazione del nome del file");
    }

    newCSVfile->headings = NULL;
    newCSVfile->headings_count = 0;
    newCSVfile->rows = NULL;
    newCSVfile->rows_count = 0;

    newCSVfile->file = fopen(filename, "a+");
    fseek(newCSVfile->file, 0, SEEK_SET);

    if (newCSVfile->file == NULL) {
        perror("Errore nell'apertura del file");
        free(newCSVfile);
        exit(EXIT_FAILURE);
    }

    return newCSVfile;
}

void csvFree(CSVFile *csv) {
    for (size_t i = 0; i < csv->headings_count; i++) {
        free(csv->headings[i]);
    }

    for (size_t i = 0; i < csv->rows_count; i++) {
        for (size_t j = 0; j < csv->headings_count; j++) {
            free(csv->rows[i][j]);
        }
        free(csv->rows[i]);
    }

    free(csv->headings);
    free(csv->rows);
    fclose(csv->file);
    free(csv);
}





// ============================== PRINT ==============================
void csvPrint(CSVFile *csv) {
    printf("Headings: %zu\n", csv->headings_count);
    for (size_t i = 0; i < csv->headings_count; i++) {
        printf("%s\n", csv->headings[i]);
    }

    printf("Rows: %zu\n", csv->rows_count);
    for (size_t i = 0; i < csv->rows_count; i++) {
        for (size_t j = 0; j < csv->headings_count; j++) { 
            printf("%s ", csv->rows[i][j]);
        }
        printf("\n");
    }
}



// ============================== GET FIELD ==============================
char *getStringField(CSVFile *csv, char *heading, size_t row) {
    for (size_t i = 0; i < csv->headings_count; i++) {
        if (strcmp(csv->headings[i], heading) == 0) {
            return csv->rows[row][i];
        }
    }
    return NULL;
}

int getIntField(CSVFile *csv, char *heading, size_t row) {
    char *field = getStringField(csv, heading, row);
    if (field == NULL) {
        fprintf(stderr, "Error: field not found\n");
    }
    
    char *endptr;
    long result = strtol(field, &endptr, 10);
    
    if (*endptr != '\0') {
        fprintf(stderr, "Warning: field contains non-numeric characters\n");
    }
    
    if (result < INT_MIN || result > INT_MAX) {
        fprintf(stderr, "Error: value out of integer range\n");
        return 0;
    }
    
    return (int)result;
}



// ============================== APPEND ==============================
void appendCsvRow(CSVFile *csv, const char *newRow) {
    char *newRowCopy = strdup(newRow);
    if (newRowCopy == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory for the new row.\n");
        return;
    }

    // Check if the newRow is valid
    size_t token_count = 0;
    char *saveptr;
    char *token = strtok_r(newRowCopy, ",", &saveptr);
    
    while (token != NULL) {
        token_count++;
        token = strtok_r(NULL, ",", &saveptr);
    }

    if (token_count != csv->headings_count) {
        fprintf(stderr, "Error: The number of tokens in the new row does not match the number of headers.\n");
        free(newRowCopy);
        return;
    }

    // Update the CSV file
    fseek(csv->file, 0, SEEK_END);
    fprintf(csv->file, "\n%s", newRow);
    fflush(csv->file);

    fseek(csv->file, 0, SEEK_SET);
    readCsvRows(csv);

    free(newRowCopy);
}





// int main() {
//     CSVFile *csv = csvInit("simple.csv");
//     readCsvHeadings(csv);
//     readCsvRows(csv);
//     csvPrint(csv);
//     appendCsvRow(csv, "test1,test2,test3");
//     csvPrint(csv);
//     csvFree(csv);
//     return 0;
// }