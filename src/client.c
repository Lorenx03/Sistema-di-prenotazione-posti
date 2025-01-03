#include "httpClient.h"
#include "userInput.h"

enum Pages {
    MAIN_MENU = 0,
    FILMS_LIST,
    BOOK_SEAT,
    CANCEL_BOOKING,
    EXIT
};

void inline printClearedResponse(char *response) {
    removeHttpHeaders(response);
    printf("\033[1J%s\n", response);
}

int countLinesOfResponse(char *response) {
    int lines = 0;
    for (int i = 0; i < strlen(response); i++) {
        if (response[i] == '\n') {
            lines++;
        }
    }
    return lines;
}

void waitForKey() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void bookSeat(TargetHost *targetHost,int film_id) {
    int currentPage = 0;
    int choice = 0;
    char buffer[4096];
    char requestBody[4096];

    do{
        switch (currentPage) {
        case 0:
            sprintf(requestBody, "%d", film_id);
            sendHttpRequest(targetHost, GET, "/book", requestBody, buffer);
            printClearedResponse(buffer);

            // User input
            printf("Scegli un orario (1-%d): ", countLinesOfResponse(buffer) - 1);
            read_int(&choice);
            break;
        }
    }while (currentPage != 4);
}


int main() {
    int currentPage = 0;
    char buffer[4096];

    TargetHost targetHost = {
        .ip_addr = "127.0.0.1",
        .portno = 8090
    };

    int choice = 0;
    
    do{
        switch (currentPage) {
        case MAIN_MENU:
            sendHttpRequest(&targetHost, GET, "/", NULL, buffer);
            printClearedResponse(buffer);

            // User input
            printf("\nInserisci la tua scelta: ");
            read_int(&choice);

            if (choice >= MAIN_MENU && choice <= EXIT) {
                currentPage = choice;
            } else{
                printf("Scelta non valida\n");
            }
            
            break;

        case FILMS_LIST:
            sendHttpRequest(&targetHost, GET, "/films", NULL, buffer);
            printClearedResponse(buffer);

            // User input
            printf("(Premi invio per tornare al menù principale)");
            waitForKey();

            currentPage = 0;
            break;

        case BOOK_SEAT:
            sendHttpRequest(&targetHost, GET, "/films/list", NULL, buffer);
            printClearedResponse(buffer);

            //count lines
            int film_count = countLinesOfResponse(buffer) - 1;

            do{
                // User input
                printf("\nPer quale film vuoi acquistare il biglietto (1-%d)(0=indietro): ", film_count);
                read_int(&choice);

                if (choice < 0 || choice > film_count) {
                    printf("Scelta non valida!\n");
                }
            }while (choice < 0 || choice > film_count);
            
            if (choice != 0){
                bookSeat(&targetHost, choice);
            }else{
                currentPage = 0;
            }
            break;
        
        case CANCEL_BOOKING:
            // sendHttpRequest(&targetHost, GET, "/cancel", buffer);
            break;

        case EXIT:
            break;
        
        default:
            break;
        }
    }while (currentPage != 4);

    return 0;
}