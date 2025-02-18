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

    if (written > 0) {
        *buffer_ptr += written;
        *remaining_size -= written;
    }
}

void generateRandomString(char *str, size_t length) {
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    size_t charset_size = sizeof(charset) - 1;
    
    for (size_t i = 0; i < length; i++) {
        str[i] = charset[rand() % charset_size];
    }
    str[length] = '\0';
}