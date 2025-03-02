#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "utils.h"
#include "httpLib.h"

// Buffer size
#define BUFFER_SIZE 4096


typedef struct targetHost {
    char *ip_addr;
    int portno;
    struct sockaddr_in server_addr;
    int sockfd;
} TargetHost;

void connectToSockServer(TargetHost *targetHost);
int sendHttpRequest(TargetHost *targetHost, HttpMethod method, char *path, char *body, char *response);
void removeHttpHeaders(char *response);

#endif // HTTPCLIENT_H