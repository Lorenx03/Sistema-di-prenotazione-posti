#include "httpClient.h"

//EXAMPLE USAGE
int main() {
    char buffer[BUFFER_SIZE];
    TargetHost targetHost = {
        .hostname = "localhost",
        .portno = 8090
    };
    
    connectToHttpServer(&targetHost);

    // Invia richiesta HTTP GET e ricevi la risposta
    sendHttpRequest(&targetHost, GET, "/films", buffer);

    // Stampa la risposta del server
    printf("%s\n", buffer);

    // Chiudi il socket
    close(targetHost.sockfd);
    return 0;
}