#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>

#include "utils.h"
#include "userInput.h"
#include "httpLib.h"

// Buffer size
#define BUFFER_SIZE 4096


typedef struct targetHost {
    char *ip_addr;
    int portno;
    struct sockaddr_in host_addr;
    int sockfd;
} TargetHost;

// Try to connect to the host, polulates the sockfd field of the targetHost struct
void connectToHost(TargetHost *targetHost);

// Send an HTTP request to the target host and store the response in the response buffer
// Returns the HTTP status code
int sendHttpRequest(TargetHost *targetHost, HttpMethod method, char *path, char *body, char *response);

// Removes the headers from an HTTP response
void removeHttpHeaders(char *response);

#endif // HTTPCLIENT_H