#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

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

void connectToHttpServer(TargetHost *targetHost);
void sendHttpRequest(TargetHost *targetHost, HttpMethod method, char *path, char *response);

#endif // HTTPCLIENT_H