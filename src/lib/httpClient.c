#include "httpClient.h"

void connectToSockServer(TargetHost *targetHost) {
    // Crea il socket
    targetHost->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (targetHost->sockfd < 0) {
        fprintf(stderr, "Errore nell'apertura del socket\n");
        exit(1);
    }

    // printf("Socket creato con fd: %d\n", targetHost->sockfd);

    targetHost->server_addr.sin_family = AF_INET;
    targetHost->server_addr.sin_port = htons(targetHost->portno);

    if (inet_pton(AF_INET, targetHost->ip_addr, &targetHost->server_addr.sin_addr) <= 0) {
        fprintf(stderr, "Errore nella conversione dell'indirizzo IP\n");
        exit(1);
    }

    if(connect(targetHost->sockfd, (struct sockaddr *)&targetHost->server_addr, sizeof(targetHost->server_addr)) < 0) {
        fprintf(stderr, "Errore nella connessione al server\n");
        exit(1);
    }
}

void removeHttpHeaders(char *response) {
    char *start = strstr(response, "\r\n\r\n");
    if (start != NULL) {
        start += 4;
        memmove(response, start, strlen(start) + 1);
    }
}


void sendHttpRequest(TargetHost *targetHost, HttpMethod method, char *path, char *body, char *response) {
    connectToSockServer(targetHost);

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
    
    if (body != NULL && strlen(body) > 0) {
        snprintf(buffer, sizeof(buffer),
                 "%s %s HTTP/1.1\r\n"
                 "Host: %s\r\n"
                 "Content-Length: %zu\r\n"
                 "Connection: close\r\n\r\n"
                 "%s",
                 methodStr, path, targetHost->ip_addr, strlen(body), body);
    } else {
        snprintf(buffer, sizeof(buffer),
                 "%s %s HTTP/1.1\r\n"
                 "Host: %s\r\n"
                 "Connection: close\r\n\r\n",
                 methodStr, path, targetHost->ip_addr);
    }
    
    if (send(targetHost->sockfd, buffer, strlen(buffer),0) < 0) {
        fprintf(stderr, "Errore durante l'invio della richiesta HTTP\n");
        exit(1);
    }

    // Leggi la risposta del server
    int bytes_received;
    int total_bytes = 0;
    while ((bytes_received = recv(targetHost->sockfd, response + total_bytes, BUFFER_SIZE - total_bytes - 1, 0)) > 0) {
        total_bytes += bytes_received;
        if (total_bytes >= BUFFER_SIZE - 1) break;
    }

    if (bytes_received < 0) {
        fprintf(stderr, "Errore nella lettura dal socket\n");
        exit(1);
    }

    response[total_bytes] = '\0';

    close(targetHost->sockfd);
    targetHost->sockfd = -1;
}

//EXAMPLE USAGE
// int main() {
//     char buffer[BUFFER_SIZE];
//     TargetHost targetHost = {
//         .hostname = "localhost",
//         .portno = 8080
//     };
    
//     connectToHttpServer(&targetHost);

//     // Invia richiesta HTTP GET e ricevi la risposta
//     sendHttpRequest(&targetHost, GET, "/hello", buffer);

//     // Stampa la risposta del server
//     printf("%s\n", buffer);

//     // Chiudi il socket
//     close(targetHost.sockfd);
//     return 0;
// }