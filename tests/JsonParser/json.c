#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <ctype.h>

typedef union jsonValue {
    char *stringValue;
    int intValue;
    float floatValue;
    bool boolValue;
    void *arrayValue;
}JsonValue;

enum type {
    STRING,
    INTEGER,
    FLOAT,
    BOOLEAN,
    ARRAY,
};

typedef struct jsonAttribute{
    JsonValue value;
    enum type valueType;
    char key[100];
}JsonAttribute;


void slice(char* str, char* result, size_t start, size_t end) {
    strncpy(result, str + start, end - start);
    result[end - start] = '\0';
}

void trim(char * s) {
    char * p = s;
    int l = strlen(p);

    while(isspace(p[l - 1])) p[--l] = 0;
    while(* p && isspace(* p)) ++p, --l;

    memmove(s, p, l + 1);
}  


JsonAttribute *parseAttribute(char *entry) {
    char *entryCpy = malloc(strlen(entry));
    strcpy(entryCpy, entry);

    trim(entryCpy);

    JsonAttribute *attribute = malloc(sizeof(JsonAttribute));
    if (attribute == NULL) {
        fprintf(stderr, "Error in memory allocation");
        exit(EXIT_FAILURE);
    }

    char* rest = entryCpy;
    char* token = strtok_r(rest, ":", &rest);
    
    strcpy(attribute->key, token);

    token = strtok_r(rest, ":", &rest);
    trim(token);

    if (token[0] == '\"') {
        attribute->value.stringValue = malloc(strlen(token));
        strcpy(attribute->value.stringValue, token);
        slice(attribute->value.stringValue, attribute->value.stringValue, 1, strlen(attribute->value.stringValue) - 1);
        attribute->valueType = STRING;
    } else if (isdigit(token[0])) {
        if (strchr(token, '.')) {
            attribute->value.floatValue = atof(token);
            attribute->valueType = FLOAT;
        } else {
            attribute->value.intValue = atoi(token);
            attribute->valueType = INTEGER;
        }
    } else if (strcmp(token, "true") == 0 || strcmp(token, "false") == 0) {
        attribute->value.boolValue = strcmp(token, "true") == 0;
        attribute->valueType = BOOLEAN;
    }

    free(entryCpy);
    return attribute;
}


void printJsonAttribute(JsonAttribute *attribute) {
    printf("Key: %s\n", attribute->key);
    switch (attribute->valueType) {
        case STRING:
            printf("Value: %s\n", attribute->value.stringValue);
            break;
        case INTEGER:
            printf("Value: %d\n", attribute->value.intValue);
            break;
        case FLOAT:
            printf("Value: %f\n", attribute->value.floatValue);
            break;
        case BOOLEAN:
            printf("Value: %s\n", attribute->value.boolValue ? "true" : "false");
            break;
    }
}


int main() {
    char *json = "{\"name\": \"John Doe\", \"age\": 30, \"city\": \"New York\"}";

    char buffer[100];
    slice(json, buffer, 1, strlen(json) - 1);

    char delimit[] = ",";
    char *token = strtok(buffer, delimit);

    while(token != NULL){
        JsonAttribute *attribute = parseAttribute(token);
        printJsonAttribute(attribute);
        token = strtok(NULL, delimit);
    }
    
    return 0;
}