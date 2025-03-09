#include "httpClient.h"
#include "userInput.h"
#include "utils.h"
#include "cinemaMap.h"
#include <arpa/inet.h>

enum Pages {
    MAIN_MENU = 0,
    FILMS_LIST,
    BOOK_SEAT,
    CANCEL_BOOKING,
    EXIT
};

// ====================== Utils ======================

void printClearedResponse(char *response) {
    removeHttpHeaders(response);
    printf("\033[1J%s\n", response);
}

int countLinesOfResponse(char *response) {
    int lines = 0;
    for (size_t i = 0; i < strlen(response); i++) {
        if (response[i] == '\n') {
            lines++;
        }
    }
    return lines;
}

void parseSeat(char *seat, char *row, int *column) {
    if(seat == NULL || row == NULL || column == NULL){
        fprintf(stderr, "parseSeat: NULL pointer\n");
        return;
    };

    if (strlen(seat) < 2 || strlen(seat) > 3) {
        fprintf(stderr, "parseSeat: Invalid seat format\n");
        seat[0] = '\0';
        *row = '0';
        *column = 0;
        return;
    }
    
    char number[3] = {0};

    *row = seat[0];
    strncpy(number, &seat[1], 2);
    *column = safeStrToInt(number);
}


// Book a seat page
void bookSeatPages(TargetHost *targetHost, int film_id) {
    char response[4096];
    char requestBody[4096];
    int currentPage = 0;

    int numberOfSeats = 0;
    int showTimeChoice = 0;
    char filmTitle[1024] = {0};
    char filmShowtime[1024] = {0};

    char userInput[1024] = {0};
    char seatChoiceRow = 0;
    int seatChoiceColumn = 0;

    char hallMap[2048] = {0}; // Map sent by the server
    char hallMapBuff[8192] = {0}; // Buffer to show to the user
    int hallMapWidth = 0;

    int hallColums = -1;
    int hallRows = -1;

    char ticketName[100] = {0};

    char *saveptr;
    char *token;

    do{
        switch (currentPage) {
        case 0:
            snprintf(requestBody, sizeof(requestBody), "%d", film_id);
            if(sendHttpRequest(targetHost, GET, "/films/showtimes", requestBody, response) != HTTP_STATUS_OK){
                printf("Errore nella richiesta\n");
                return;
            }

            printClearedResponse(response);

            do {
                // User input
                printf("Scegli un orario (1-%d)(0=indietro): ", countLinesOfResponse(response) - 1);
                read_int(&showTimeChoice);

                if (showTimeChoice >= 1 && showTimeChoice <= countLinesOfResponse(response) - 1) {
                    {
                        getLine(response, showTimeChoice+1, filmShowtime, sizeof(filmShowtime));
                        memmove(filmShowtime, filmShowtime + 3, strlen(filmShowtime));
                        char *ptr = strstr(response, "per: ");
                        if (ptr) {
                            ptr += 5; 
                            while (*ptr == ' ') ptr++;
                            ptr[strcspn(ptr,"\n")] = 0;
                            strncpy(filmTitle, ptr, sizeof(filmTitle)-1);
                        }
                    }
                    currentPage = 1;
                } else if (showTimeChoice == 0) {
                    currentPage = 4;
                }else{
                    printf("Scelta non valida\n");
                }
            } while (!currentPage);
            break;

        case 1:
            // SECTION: Book seat prompts

            snprintf(requestBody, sizeof(requestBody), "%d.%d", film_id, showTimeChoice);
            if(sendHttpRequest(targetHost, GET, "/films/map", requestBody, response) != HTTP_STATUS_OK){
                printf("Errore nella richiesta\n");
                return;
            }
            removeHttpHeaders(response);

            token = strtok_r(response, ".", &saveptr);
            if (token != NULL) {
                hallRows = safeStrToInt(token);
                token = strtok_r(NULL, ".", &saveptr);
                if (token != NULL) {
                    hallColums = safeStrToInt(token);
                    token = strtok_r(NULL, ".", &saveptr);
                    if (token != NULL) {
                        strncpy(hallMap, token, sizeof(hallMap)-1);
                        hallMapWidth = generateHallMap(hallMap, hallMapBuff, sizeof(hallMapBuff), hallRows, hallColums);
                    }
                }
            }

            if (hallColums == -1 || hallRows == -1 || strlen(hallMapBuff) == 0) {
                printf("Errore nella lettura della mappa della sala\n");
                return;
            }
            
            printf("\033[1J\n");
            centerMapText(hallMapWidth+14, "%s - %s", filmTitle, filmShowtime);
            printf("\n\n");
            centerMapText(hallMapWidth, "Sala %d", film_id);
            printf("\n\n");
            printf("%s\n", hallMapBuff);
            printf("\nLegenda: \n\033[0;32m[A1]\033[0m Disponibile \n\033[0;31m[A1]\033[0m Prenotato \n\033[0;34m[A1]\033[0m Disabili\n\n");

            // DEBUG:
            // printf("Film: %s\n", filmTitle);
            // printf("Orario: %s\n", filmShowtime);
            // printf("righe: %d, colonne: %d\n", hallRows, hallColums);

            while(1){
                printf("Quanti posti vuoi prenotare (1-%d)(0=indietro)? ", 4);
                read_int(&numberOfSeats);

                if (numberOfSeats >= 1 && numberOfSeats <= 4) {
                    break;
                }else if (numberOfSeats == 0){
                    currentPage = 0;
                    break;
                }else{
                    printf("Numero di posti non valido\n");
                }
            }

            if (currentPage == 0) {
                break;
            }
            

            for (int i = 1; i <= numberOfSeats; i++){
                printf("\033[1J\n");
                centerMapText(hallMapWidth+14, "%s - %s", filmTitle, filmShowtime);
                printf("\n\n");
                centerMapText(hallMapWidth, "Sala %d", film_id);
                printf("\n\n");
                printf("%s\n", hallMapBuff);
                // Legend
                printf("Legenda: \n\033[0;32m[A1]\033[0m Disponibile \n\033[0;31m[A1]\033[0m Prenotato \n\033[0;34m[A1]\033[0m Disabili\n\033[0;33m{A1}\033[0m Selezionato\n\n");
                
                while (1){
                    printf("Inserisci la riga e la colonna del posto %d (Es: A7): ", i);
                    read_str(userInput);
                    convertToUppercase(userInput);
                    parseSeat(userInput, &seatChoiceRow, &seatChoiceColumn);

                    if (
                        seatChoiceRow >= 'A' &&
                        seatChoiceRow <= ('A'+ hallRows) && 
                        seatChoiceColumn >= 1 &&
                        seatChoiceColumn <= hallColums
                    ) {
                        if(hallMap[(seatChoiceRow - 'A') * hallColums + seatChoiceColumn - 1] == '1'){
                            printf("Posto già prenotato\n");
                            continue;
                        }else if(hallMap[(seatChoiceRow - 'A') * hallColums + seatChoiceColumn - 1] == '3'){
                            printf("Posto già selezionato\n");
                            continue;
                        }
                        break;
                    }else{
                        printf("Posto non valido\n");
                        continue;
                    }
                } 
                
                hallMap[(seatChoiceRow - 'A') * hallColums + seatChoiceColumn - 1] = '3'; //SELECTED
                generateHallMap(hallMap, hallMapBuff, sizeof(hallMapBuff), hallRows, hallColums);

                size_t remaining = sizeof(requestBody) - strlen(requestBody) - 1;
                strncat(requestBody, ".", remaining);
                remaining = sizeof(requestBody) - strlen(requestBody) - 1;
                strncat(requestBody, userInput, remaining);

                printf("Posto %d: %s\n", i, userInput);
            }

            printf("\033[1J\n");
            centerMapText(hallMapWidth+14, "%s - %s", filmTitle, filmShowtime);
            printf("\n\n");
            centerMapText(hallMapWidth, "Sala %d", film_id);
            printf("\n\n");
            printf("%s\n", hallMapBuff);
            printf("Legenda: \n\033[0;32m[A1]\033[0m Disponibile \n\033[0;31m[A1]\033[0m Prenotato \n\033[0;34m[A1]\033[0m Disabili\n\033[0;33m{A1}\033[0m Selezionato\n\n");

            while (1){
                printf("Confermi la prenotazione? (s/n): ");
                read_str(userInput);
                convertToUppercase(userInput);

                if (strcmp(userInput, "S") == 0 || strcmp(userInput, "N") == 0) {
                    break;
                }else{
                    printf("Scelta non valida\n");
                }
            }

            if (strcmp(userInput, "N") == 0) {
                for (int i = 0; i < hallRows; i++){
                    for (int j = 0; j < hallColums; j++){
                        if (hallMap[i * hallColums + j] == '3') {
                            hallMap[i * hallColums + j] = '1';
                        }
                    }
                }
                break;
            }

            // NOTE - POST /book
            printf("\033[1J\n");
            if(sendHttpRequest(targetHost, POST, "/book", requestBody, response) != HTTP_STATUS_CREATED){
                removeHttpHeaders(response);
                printf("%s\n", response);
                printf("(Premi invio per riprovare)");
                waitForKey();
                break;
            }
            removeHttpHeaders(response);

            getLine(response, 1, ticketName, sizeof(ticketName));
            memmove(response, response + strlen(ticketName) + 1, strlen(response) - strlen(ticketName));
            strncat(ticketName, ".txt", sizeof(ticketName) - strlen(ticketName) - 1);

            printf("%s\n", response);

            while (1){
                printf("Vuoi salvare la prenotazione? (s/n): ");
                read_str(userInput);
                convertToUppercase(userInput);

                if (strcmp(userInput, "S") == 0 || strcmp(userInput, "N") == 0) {
                    break;
                }else{
                    printf("Scelta non valida\n");
                }
            }

            if (strcmp(userInput, "S") == 0) {
                FILE *file = fopen(ticketName, "w");
                if (file == NULL) {
                    perror("Error opening file");
                    return;
                }
                fprintf(file, "%s", response);

                fflush(file);
                fclose(file);
                printf("Prenotazione salvata con nome: %s\n\n", ticketName);
            }
            
            printf("(Premi invio per tornare al menù principale)");
            waitForKey();

            currentPage = 4;
            
            break;
            // !SECTION
        }
    }while (currentPage != 4);
}



void unBookSeatPage(TargetHost *targetHost) {
    char response[4096];
    int currentPage = -1;
    char seatCode[1024] = {0};

    do{
        switch (currentPage) {
            case 0:
            break;

            case -1:
                printf("\033[1J\n");
                printf("\033[1;31mCancellazione prenotazione\033[0m\n");
                printf("==========================\n");
                printf(" 0. Indietro\n");
                printf(" 1. Singolo posto\n");
                printf(" 2. Prenotazione intera\n\n");

                printf("Inserisci la tua scelta: ");
                read_int(&currentPage);
            break;

            case 1:
                printf("\033[1J\n");
                printf("\033[1;31mCancellazione prenotazione -> Singolo posto\033[0m\n");
                printf("Inserisci il codice del sedile Es: 80MMSYNB-5698X8B8\n\n");
                printf("Codice (0 = indietro): ");
                read_str(seatCode);

                if (strcmp(seatCode, "0") == 0) {
                    currentPage = -1;
                    break;
                }

                if (strlen(seatCode) != 17 || strcspn(seatCode, "-") != 8) {
                    printf("\033[1;31mCodice non valido\033[0m\n");
                    printf("(Premi invio per riprovare)");
                    waitForKey();
                    currentPage = 1;
                    break;
                }

                if(sendHttpRequest(targetHost, POST, "/unbook", seatCode, response) != HTTP_STATUS_OK){
                    removeHttpHeaders(response);
                    printf("\033[1;31mErrore nella richiesta: %s\033[0m\n", response);
                    printf("(Premi invio per riprovare)");
                    waitForKey();
                    currentPage = -1; // going back to the selection menu
                    break;
                }
                
                printf("\033[1J\n");
                removeHttpHeaders(response);
                printf("\033[0;32m%s\033[0m\n\n", response);

                printf("(Premi invio per tornare al menù principale)");
                waitForKey();
                currentPage = 0;
            break;

            case 2:
                printf("\033[1J\n");
                printf("\033[1;31mCancellazione prenotazione -> Prenotazione Intera\033[0m\n");
                printf("Inserisci il codice della prenotazione Es: (80MMSYNB)-XXXXXXXX (solo prime 8 cifre)\n\n");
                printf("Codice (0 = indietro): ");
                read_str(seatCode);

                if (strcmp(seatCode, "0") == 0) {
                    currentPage = -1;
                    break;
                }

                if (strlen(seatCode) != 8) {
                    printf("\033[1;31mCodice non valido\033[0m\n");
                    printf("(Premi invio per riprovare)");
                    waitForKey();
                    currentPage = -1;
                    break;
                }

                if(sendHttpRequest(targetHost, POST, "/unbook", seatCode, response) != HTTP_STATUS_OK){
                    removeHttpHeaders(response);
                    printf("\033[1;31mErrore nella richiesta: %s\033[0m\n", response);
                    printf("(Premi invio per riprovare)");
                    waitForKey();
                    currentPage = -1;
                    break;
                }
                
                printf("\033[1J\n");
                removeHttpHeaders(response);
                printf("\033[0;32m%s\033[0m\n\n", response);

                printf("(Premi invio per tornare al menù principale)");
                waitForKey();
                currentPage = 0;
            break;

            default:
                printf("\033[1J\n");
                printf("Scelta non valida\n");
                printf("(Premi invio per rirpovare)");
                waitForKey();
                currentPage = -1;
            break;
        }
    }while (currentPage != 0);
}

// ./client -a <ip> -p <port>
int main(int argc, char *argv[]) {
    // Default values
    char ip_addr[16] = "127.0.0.1";
    int port = 8090;

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0) {
            printf("Usage: %s [-a address] [-p port]\n\n", argv[0]);
            printf("Options:\n");
            printf("  -h\t\tShow this help message\n");
            printf("  -a <address>\tSpecify server IP address (default: 127.0.0.1)\n");
            printf("  -p <port>\tSpecify server port (default: 8090)\n");
            return 0;
        } else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            port = atoi(argv[i + 1]);
            if (port <= 0 || port > 65535) {
                fprintf(stderr, "Invalid port number. Must be between 1 and 65535\n");
                return 1;
            }
            i++;
        } else if (strcmp(argv[i], "-a") == 0 && i + 1 < argc) {
            struct sockaddr_in sa;
            if (inet_pton(AF_INET, argv[i + 1], &(sa.sin_addr)) <= 0) {
                fprintf(stderr, "Invalid IP address\n"); 
                return 1;
            }
            strncpy(ip_addr, argv[i + 1], sizeof(ip_addr) - 1);
            i++;
        }
    }

    int currentPage = 0;
    char responseBuffer[4096];

    // Cinema server address
    TargetHost targetHost = {
        .ip_addr = ip_addr,
        .portno = port
    };

    int choice = 0;
    
    do{
        switch (currentPage) {
        case MAIN_MENU:
            if(sendHttpRequest(&targetHost, GET, "/", NULL, responseBuffer) != HTTP_STATUS_OK){
                printf("Errore nella richiesta\n");
                return 1;
            }
            
            printClearedResponse(responseBuffer);

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
            if(sendHttpRequest(&targetHost, GET, "/films", NULL, responseBuffer) != HTTP_STATUS_OK){
                printf("Errore nella richiesta\n");
                return 1;
            }
            printClearedResponse(responseBuffer);

            // User input
            printf("(Premi invio per tornare al menù principale)");
            waitForKey();

            currentPage = 0;
            break;

        case BOOK_SEAT:
            if(sendHttpRequest(&targetHost, GET, "/films/list", NULL, responseBuffer ) != HTTP_STATUS_OK){
                printf("Errore nella richiesta\n");
                return 1;
            }
            printClearedResponse(responseBuffer);

            //count lines
            int film_count = countLinesOfResponse(responseBuffer) - 1;

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
            }

            currentPage = 0;
            break;
        
        case CANCEL_BOOKING:
            unBookSeatPage(&targetHost);
            currentPage = 0;
            break;

        case EXIT:
            break;
        
        default:
            break;
        }
    }while (currentPage != 4);

    return 0;
}