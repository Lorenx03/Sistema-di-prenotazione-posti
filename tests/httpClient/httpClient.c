#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

// Buffer size
#define BUFFER_SIZE 4096

typedef enum httpMethod {
    GET,
    POST,
    PUT,
    DELETE,
    HEAD,
    OPTIONS,
    TRACE,
    CONNECT,
    PATCH,
    UNKNOWN
} HttpMethod;

typedef struct targetHost {
    char *hostname;
    int portno;
    int sockfd;
} TargetHost;

// Function to handle errors
void error(const char *msg) {
    perror(msg);
    exit(1);
}

void connectToHttpServer(TargetHost *targetHost) {
    struct sockaddr_in server_addr;
    struct hostent *server;

    // Crea il socket
    targetHost->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (targetHost->sockfd < 0) {
        error("Errore nell'apertura del socket");
    }

    // Ottiene il server tramite hostname
    server = gethostbyname(targetHost->hostname);
    if (server == NULL) {
        error("Host non trovato");
    }

    // Imposta l'indirizzo del server
    memset((char *)&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memcpy((char *)&server_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
    server_addr.sin_port = htons(targetHost->portno);

    // Connette al server
    if (connect(targetHost->sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        error("Errore nella connessione al server");
    }

    // Non ritorna nulla, il campo sockfd di targetHost è già stato modificato
}

void sendHttpRequest(TargetHost *targetHost, HttpMethod method, char *path, char *response) {
    char buffer[BUFFER_SIZE];
    const char *methodStr;
    
    switch (method) {
        case GET:
            methodStr = "GET";
            break;
        case POST:
            methodStr = "POST";
            break;
        case PUT:
            methodStr = "PUT";
            break;
        case DELETE:
            methodStr = "DELETE";
            break;
        case HEAD:
            methodStr = "HEAD";
            break;
        case OPTIONS:
            methodStr = "OPTIONS";
            break;
        case TRACE:
            methodStr = "TRACE";
            break;
        case CONNECT:
            methodStr = "CONNECT";
            break;
        case PATCH:
            methodStr = "PATCH";
            break;
        default:
            methodStr = "UNKNOWN";
            break;
    }
    
    snprintf(buffer, sizeof(buffer),
             "%s /%s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Connection: close\r\n\r\n",
             methodStr, path, targetHost->hostname);
    
    if (write(targetHost->sockfd, buffer, strlen(buffer)) < 0) {
        error("Errore durante l'invio della richiesta HTTP");
    }

    // Leggi la risposta del server
    int bytes_received;
    int total_bytes = 0;
    while ((bytes_received = read(targetHost->sockfd, response + total_bytes, BUFFER_SIZE - total_bytes - 1)) > 0) {
        total_bytes += bytes_received;
        if (total_bytes >= BUFFER_SIZE - 1) break;
    }

    if (bytes_received < 0) {
        error("Errore nella lettura dal socket");
    }

    response[total_bytes] = '\0';
}


int main() {
    char buffer[BUFFER_SIZE];
    TargetHost targetHost = {
        .hostname = "www.example.com",
        .portno = 80
    };
    
    connectToHttpServer(&targetHost);

    // Invia richiesta HTTP GET e ricevi la risposta
    sendHttpRequest(&targetHost, GET, "/", buffer);

    // Stampa la risposta del server
    printf("%s", buffer);

    // Chiudi il socket
    close(targetHost.sockfd);
    return 0;
}