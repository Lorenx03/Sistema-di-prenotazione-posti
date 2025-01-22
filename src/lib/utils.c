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