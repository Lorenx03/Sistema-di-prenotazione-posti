#ifndef CSVLIB_H
#define CSVLIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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
void readCsvHeadings(CSVFile *csv);
void readCsvRows(CSVFile *csv);

// ============================== APPEND ==============================
void appendCsvRow(CSVFile *csv, const char *newRow);

// ============================== PRINT ==============================
void csvPrint(CSVFile *csv);

// ============================== INIT ==============================
CSVFile *csvInit(const char *filename);

// ============================== FREE ==============================
void csvFree(CSVFile *csv);

char *getStringField(CSVFile *csv, char *heading, size_t row);
int getIntField(CSVFile *csv, char *heading, size_t row);


#endif