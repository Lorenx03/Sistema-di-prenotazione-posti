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