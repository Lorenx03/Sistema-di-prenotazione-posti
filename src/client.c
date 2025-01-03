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

void waitForKey() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
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
            sendHttpRequest(&targetHost, GET, "/", buffer);
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
            sendHttpRequest(&targetHost, GET, "/films", buffer);
            printClearedResponse(buffer);

            // User input
            printf("(Premi invio per tornare al menù principale)");
            waitForKey();

            currentPage = 0;
            break;

        case BOOK_SEAT:
            // sendHttpRequest(&targetHost, GET, "/book", buffer);
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