#include "utils.h"

int safeStrToInt(char *buff){
    bool success = false;
    int number = 0;
    errno = 0;
    char *endptr;

    number = strtol(buff, &endptr,10);

    if(errno == ERANGE){
        success = false;
    }else if(endptr == buff){
        success = false;
    }else if(*endptr && *endptr != '\n'){
        success = false;
    }else{
        success = true;
    }

    if (success){
        return number;
    }else{
        return -1;
    }
}

void getLine(char* str, int n, char* buffer, size_t buffer_size) {
    int current_line = 1;
    const char* start = str;
    const char* end;

    while (*start && current_line < n) {
        if (*start == '\n') {
            current_line++;
        }
        start++;
    }

    if (current_line == n) {
        end = strchr(start, '\n');
        if (!end) {
            end = str + strlen(str);
        }
        size_t len = end - start;
        if (len >= buffer_size) {
            len = buffer_size - 1;
        }
        strncpy(buffer, start, len);
        buffer[len] = '\0';
    } else {
        buffer[0] = '\0';
    }    
}


void appendToBuffer(char **buffer_ptr, size_t *remaining_size, const char *format, ...) {
    va_list args;
    va_start(args, format);
    
    int written = vsnprintf(*buffer_ptr, *remaining_size, format, args);
    va_end(args);
    
    if (written > 0 && (size_t)written <= *remaining_size) {
        *buffer_ptr += written;
        *remaining_size -= written;
    } else if (written > 0 && *remaining_size > 0) {
        // Partial write occurred - advance only by what actually fit
        *buffer_ptr += (*remaining_size - 1); // -1 for the null terminator
        *remaining_size = 0;
    }
    // If written <= 0, don't modify pointers (error occurred)
}

void generateRandomString(char *str, size_t length) {
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    size_t charset_size = sizeof(charset) - 1;
    
    for (size_t i = 0; i < length; i++) {
        str[i] = charset[rand() % charset_size];
    }
    str[length] = '\0';
}


void getNthToken(char *str, char *delim, int n, char *token, size_t token_size) {
    char temp[1024] = {0};
    strncpy(temp, str, sizeof(temp)-1);

    char *saveptr;
    char *current_token;
    int current_token_index = 0;

    current_token = strtok_r(temp, delim, &saveptr);
    while (current_token != NULL && current_token_index < n) {
        current_token = strtok_r(NULL, delim, &saveptr);
        current_token_index++;
    }

    memset(token, 0, token_size);

    if (current_token != NULL) {
        if(strlen(current_token) < token_size){
            strncpy(token, current_token, token_size);
        }else{
            fprintf(stderr, "getNToken: token too long\n");
        }
    } else {
        fprintf(stderr, "getNToken: token not found\n");
    }
}


void convertToUppercase(char *str){
    if (str == NULL) {
        fprintf(stderr, "convertToUppercase: NULL pointer provided\n");
        return;
    }

    size_t len = strlen(str);
    if (len == 0) {
        fprintf(stderr, "convertToUppercase: string is empty\n");
        return;
    }

    for (size_t i = 0; i < len; i++) {
        if (str[i] >= 'a' && str[i] <= 'z') {
            str[i] = str[i] - 'a' + 'A';
        }
    }
}


int fdeleteBytes(FILE* fp, int bytes) {
    // to store each char from file
    char byte;

    long readPos = ftell(fp) + bytes;
    long writePos = ftell(fp);
    long startingPos = writePos;

    // start reading from the position which comes after the bytes to be deleted
    fseek(fp, readPos, SEEK_SET);

    while (fread(&byte, sizeof(byte), 1, fp)) {
        readPos = ftell(fp);
        fseek(fp, writePos, SEEK_SET);
        
        if (fwrite(&byte, sizeof(byte), 1, fp) == 0) 
            return errno;

        writePos = ftell(fp);
        fseek(fp, readPos, SEEK_SET);
    }

    ftruncate(fileno(fp), writePos);
    fseek(fp, startingPos, SEEK_SET); 
    return 0;
}