#include "httpClient.h"
#include "userInput.h"
#include "utils.h"

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

int inline countLinesOfResponse(char *response) {
    int lines = 0;
    for (size_t i = 0; i < strlen(response); i++) {
        if (response[i] == '\n') {
            lines++;
        }
    }
    return lines;
}

void bookSeatPages(TargetHost *targetHost, int film_id) {
    int currentPage = 0;

    int numberOfSeats = 0;
    int showTimeChoice = 0;
    char seatChoice[1024] = {0};


    char response[4096];
    char requestBody[4096];

    char hallInfo[6] = {0};
    size_t hallInfoOffset = 0;
    int hallColums = -1;
    int hallRows = -1;

    char *saveptr;
    char *token;

    do{
        switch (currentPage) {
        case 0:
            snprintf(requestBody, sizeof(requestBody), "%d", film_id);
            sendHttpRequest(targetHost, GET, "/book", requestBody, response);
            printClearedResponse(response);

            do {
                // User input
                printf("Scegli un orario (1-%d): ", countLinesOfResponse(response) - 1);
                read_int(&showTimeChoice);

                if (showTimeChoice >= 1 && showTimeChoice <= countLinesOfResponse(response) - 1) {
                    currentPage = 1;
                } else {
                    printf("Scelta non valida\n");
                }
            } while (!currentPage);
            break;

        case 1:
            snprintf(requestBody, sizeof(requestBody), "%d.%d.", film_id, showTimeChoice);
            sendHttpRequest(targetHost, GET, "/films/map", requestBody, response);
            removeHttpHeaders(response);

            hallInfoOffset = strcspn(response, "\n");
            if(hallInfoOffset <= 5 && hallInfoOffset >= 3){
                strncpy(hallInfo, response, hallInfoOffset);
                memmove(response, response + hallInfoOffset + 1, strlen(response) - hallInfoOffset);

                token = strtok_r(hallInfo, ".", &saveptr);
                if (token != NULL) {
                    hallRows = safeStrToInt(token);
                    token = strtok_r(NULL, ".", &saveptr);
                    if (token != NULL) {
                        hallColums = safeStrToInt(token);
                    }
                }
            }else{
                printf("Errore nella lettura delle informazioni della sala\n");
                currentPage = 0;
                break;
            }

            printf("\033[1J%s\n", response);
            printf("righe: %d, colonne: %d\n", hallRows, hallColums);

            while(1){
                printf("Quanti posti vuoi prenotare (1-%d)? ", 4);
                read_int(&numberOfSeats);

                if (numberOfSeats >= 1 && numberOfSeats <= 4) {
                    break;
                }else{
                    printf("Numero di posti non valido\n");
                }
            }

            for (int i = 1; i <= numberOfSeats; i++){
                while (1){
                    printf("Inserisci la riga e la colonna del posto %d (Es: A7): ", i);
                    read_str(seatChoice);

                    if (
                        strlen(seatChoice) == 2 && 
                        seatChoice[0] >= 'A' &&
                        seatChoice[0] <= ('A'+ hallRows) && 
                        safeStrToInt(&seatChoice[1]) >= 1 &&
                        safeStrToInt(&seatChoice[1]) <= hallColums
                    ) {
                        break;
                    }else{
                        printf("Posto non valido\n");
                        continue;
                    }
                } 
                
                strncat(requestBody, seatChoice, 2);
                printf("Posto %d: %s\n", i, seatChoice);
            }

            printf("Richiesta: %s\n", requestBody);
            waitForKey();
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
                bookSeatPages(&targetHost, choice);
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