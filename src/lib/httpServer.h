#ifndef HTTP_WEB_SERVER_H
#define HTTP_WEB_SERVER_H

#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/sem.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdatomic.h>

#include "userInput.h"
#include "utils.h"
#include "httpLib.h"

#define MAX_ROUTE_NAME 100
#define MAX_REQUEST_SIZE 4096
#define MAX_RESPONSE_SIZE 4096
#define BUFFER_SIZE 1024
// #define POLL_FD_SIZE 200

#define HTTP_RESPONSE_TEMPLATE "HTTP/1.1 %d %s\r\n" \
    "Content-Type: %s\r\n" \
    "Content-Length: %zu\r\n" \
    "\r\n" \
    "%s"

// ================================ ROUTES ================================
/* 
This is the tree structure for the HTTP routes, each hadler has to be set using the 
httpRoute->handlers array, the index of the array is the HTTP method, for example:

httpRoute->handlers[GET] = GETrootHandler;

The tree structure is used to find the correct route based on the path of the request.
*/

// Http route struct
typedef struct httpRoute {
    char *name;
    void (*handlers[9])(char*, char*);

    // Http route tree structure
    struct httpRoute *children;
    struct httpRoute *parent;
    struct httpRoute *sibling;
    struct httpRoute *child;
} HttpRoute;


// ================================ HTTP SERVER ================================

// Http request struct
typedef struct parsedHttpRequest {
    HttpMethod method;
    char path[MAX_ROUTE_NAME];
    char body[MAX_REQUEST_SIZE];
} ParsedHttpRequest;

typedef struct cronJob {
    void (*job)(void);   
    int interval;
} CronJob;

typedef struct httpServerCronJobs {
    CronJob *jobs;
    int numJobs;
} HttpServerCronJobs;

// Http server struct
typedef struct httpServer {
    short port;
    short numThreads;
    HttpServerCronJobs *cronJobs;
    HttpRoute *root;
} HttpServer;

// Worker thread params
typedef struct workerThreadParams {
    int id;
    int serverSocket;
    HttpRoute *root;
} WorkerThreadParams;

// Adds a new child route to an existing route in the HTTP routing tree
void addHttpSubroute(HttpRoute *subTreeRoot, HttpRoute *newChild);

// Finds a route in the HTTP routing tree based on the given path
HttpRoute *findHttpRoute(HttpRoute *currentNode, char *path);

// Builds an HTTP response with the given status code, message, and body
void httpResponseBuilder(char *response, int statusCode, const char *statusMessage, const char *responseBody);

void httpResponseBuilderHTML(char *response, int statusCode, const char *statusMessage, const char *responseBody);

// Generates an HTTP error response with the specified error code
void errorResponse(char *response, int errorCode);

// Parses an HTTP request string into a ParsedHttpRequest structure
void parseHttpRequest(char *request, ParsedHttpRequest *parsedRequest);

// Sets a socket to non-blocking mode
void setNonBlocking(int socket);

// Handles an individual client connection
int handleClient(int connSocketFd, HttpRoute *root);

// Main routine for worker threads handling client connections
void *workerRoutine(void *params);

// Initializes and starts the HTTP server
int httpServerServe(HttpServer *server);

#endif


// Example usage for a route handler:
// void GETrootHandler(char *request, char *response) {
//     char response_body[100];
//     snprintf(response_body, sizeof(response_body), "Root - Thread ID: %ld", (long)pthread_self());
//     httpResponseBuilder(response, 200, "OK", response_body);
// }