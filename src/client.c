#include "httpClient.h"
#include "userInput.h"
#include "utils.h"
#include "cinemaMap.h"

enum Pages {
    MAIN_MENU = 0,
    FILMS_LIST,
    BOOK_SEAT,
    CANCEL_BOOKING,
    EXIT
};

// Utils

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

void parseSeat(char *seat, char *row, int *column) {
    if(seat == NULL || row == NULL || column == NULL){
        fprintf(stderr, "parseSeat: NULL pointer\n");
        return;
    };

    if (strlen(seat) < 2 || strlen(seat) > 3) {
        fprintf(stderr, "parseSeat: Invalid seat format\n");
        strncpy(seat, "000", 1);
        *row = '0';
        *column = 0;
        return;
    }
    
    char number[3] = {0};

    *row = seat[0];
    strncpy(number, &seat[1], 2);
    *column = safeStrToInt(number);
}


// Book a seat
void bookSeatPages(TargetHost *targetHost, int film_id) {
    char response[4096];
    char requestBody[4096];
    int currentPage = 0;

    int numberOfSeats = 0;
    int showTimeChoice = 0;
    char filmTitle[1024] = {0};
    char filmShowtime[1024] = {0};

    char seatChoice[1024] = {0};
    char seatChoiceRow = 0;
    int seatChoiceColumn = 0;

    char hallMap[2048] = {0};
    char hallMapBuff[4096] = {0};
    char hallMapMostWideLine[512] = {0};

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
                            strncpy(filmTitle, ptr, sizeof(filmTitle));
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
            sendHttpRequest(targetHost, GET, "/films/map", requestBody, response);
            removeHttpHeaders(response);

            token = strtok_r(response, ".", &saveptr);
            if (token != NULL) {
                hallRows = safeStrToInt(token);
                token = strtok_r(NULL, ".", &saveptr);
                if (token != NULL) {
                    hallColums = safeStrToInt(token);
                    token = strtok_r(NULL, ".", &saveptr);
                    if (token != NULL) {
                        strncpy(hallMap, token, sizeof(hallMap));
                        generateHallMap(hallMap, hallMapBuff, sizeof(hallMapBuff), hallRows, hallColums);
                    }
                }
            }

            if (hallColums == -1 || hallRows == -1 || strlen(hallMapBuff) == 0) {
                printf("Errore nella lettura della mappa della sala\n");
                currentPage = 0;
                return;
            }
            
            getLine(hallMapBuff, 2, hallMapMostWideLine, sizeof(hallMapMostWideLine));

            printf("\033[1J\n");
            centerMapText(strlen(hallMapMostWideLine)+13, "%s - %s", filmTitle, filmShowtime);
            printf("\n\n");
            centerMapText(strlen(hallMapMostWideLine), "Sala %d", film_id);
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
                centerMapText(strlen(hallMapMostWideLine)+13, "%s - %s", filmTitle, filmShowtime);
                printf("\n\n");
                centerMapText(strlen(hallMapMostWideLine), "Sala %d", film_id);
                printf("\n\n");
                printf("%s\n", hallMapBuff);
                // Legend
                printf("Legenda: \n\033[0;32m[A1]\033[0m Disponibile \n\033[0;31m[A1]\033[0m Prenotato \n\033[0;34m[A1]\033[0m Disabili\n\033[0;33m{A1}\033[0m Selezionato\n\n");
                
                while (1){
                    printf("Inserisci la riga e la colonna del posto %d (Es: A7): ", i);
                    read_str(seatChoice);
                    convertToUppercase(seatChoice);
                    parseSeat(seatChoice, &seatChoiceRow, &seatChoiceColumn);

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
                strncat(requestBody, ".", 1);
                strncat(requestBody, seatChoice, strlen(seatChoice));
                printf("Posto %d: %s\n", i, seatChoice);
            }

            printf("\033[1J\n");
            centerMapText(strlen(hallMapMostWideLine)+13, "%s - %s", filmTitle, filmShowtime);
            printf("\n\n");
            centerMapText(strlen(hallMapMostWideLine), "Sala %d", film_id);
            printf("\n\n");
            printf("%s\n", hallMapBuff);
            printf("Legenda: \n\033[0;32m[A1]\033[0m Disponibile \n\033[0;31m[A1]\033[0m Prenotato \n\033[0;34m[A1]\033[0m Disabili\n\033[0;33m{A1}\033[0m Selezionato\n\n");

            while (1){
                printf("Confermi la prenotazione? (s/n): ");
                read_str(seatChoice);
                convertToUppercase(seatChoice);

                if (strcmp(seatChoice, "S") == 0 || strcmp(seatChoice, "N") == 0) {
                    break;
                }else{
                    printf("Scelta non valida\n");
                }
            }

            if (strcmp(seatChoice, "N") == 0) {
                for (int i = 0; i < hallRows; i++){
                    for (int j = 0; j < hallColums; j++){
                        if (hallMap[i * hallColums + j] == '3') {
                            hallMap[i * hallColums + j] = '1';
                        }
                    }
                }
                break;
            }

            printf("\033[1J\n");
            sendHttpRequest(targetHost, POST, "/book", requestBody, response);
            removeHttpHeaders(response);

            getLine(response, 1, ticketName, sizeof(ticketName));
            memmove(response, response + strlen(ticketName) + 1, strlen(response) - strlen(ticketName));
            strncat(ticketName, ".txt", 4);

            printf("%s\n", response);

            while (1){
                printf("Vuoi salvare la prenotazione? (s/n): ");
                read_str(seatChoice);
                convertToUppercase(seatChoice);

                if (strcmp(seatChoice, "S") == 0 || strcmp(seatChoice, "N") == 0) {
                    break;
                }else{
                    printf("Scelta non valida\n");
                }
            }

            if (strcmp(seatChoice, "S") == 0) {
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
            // !SECTION

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
                bookSeatPages(&targetHost, choice);
                currentPage = 0;
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