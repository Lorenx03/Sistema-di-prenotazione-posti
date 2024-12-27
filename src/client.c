#include "httpClient.h"
#include "userInput.h"

//EXAMPLE USAGE
int main() {
    int currentPage = 0;
    char buffer[4096];

    TargetHost targetHost = {
        .ip_addr = "127.0.0.1",
        .portno = 8090
    };

    do{
        connectToHttpServer(&targetHost);
        
        switch (currentPage) {
        case 0:
            sendHttpRequest(&targetHost, GET, "/", buffer);
            break;

        case 1:
            sendHttpRequest(&targetHost, GET, "/films", buffer);
            break;
        
        default:
            break;
        }

        printf("%s\n", buffer);
        printf("\nInserisci la tua scelta: ");
        read_int(&currentPage);
    }while (currentPage != 4);

    // Chiudi il socket
    close(targetHost.sockfd);
    return 0;
}