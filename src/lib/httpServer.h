#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <fcntl.h>
#include <poll.h>
#include <fcntl.h>

#define MAX_ROUTE_NAME 100
#define BUFFER_SIZE 1024

// Struttura per la coda delle connessioni
typedef struct connectionQueue {
    int *sockets;
    int front;
    int rear;
    int count;
    int size;
    pthread_mutex_t lock;
    pthread_cond_t cond;
} ConnectionQueue;


// Enumerazione dei metodi HTTP
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

// Struttura per le rotte HTTP
typedef struct httpRoute {
    char name[MAX_ROUTE_NAME];
    void (*handlers[9])(char*, char*);

    // Struttura ad albero per le rotte HTTP
    struct httpRoute *children;
    struct httpRoute *parent;
    struct httpRoute *sibling;
    struct httpRoute *child;
} HttpRoute;


typedef struct httpRoutes {
    HttpRoute *root;
} HttpRoutes;


// Parametri per il server HTTP
typedef struct httpServerParams {
    short port;
    short numThreads;
    short maxClientsPerThread;
    HttpRoutes *routes;
} HttpServerParams;

// Parametri per i thread dei lavoratori
typedef struct workerThreadParams {
    ConnectionQueue *queue;
    int maxClientsPerThread;
    HttpRoutes *routes;
} WorkerThreadParams;



// Struttura per la richiesta HTTP
typedef struct parsedHttpRequest {
    HttpMethod method;
    char path[MAX_ROUTE_NAME];
    char *body;
} ParsedHttpRequest;



// Dichiarazione delle funzioni
void initQueue(ConnectionQueue *queue, int size);
void enqueue(ConnectionQueue *queue, int socket);
int dequeue(ConnectionQueue *queue);
void destroyQueue(ConnectionQueue *queue);
void setNonBlocking(int socket);
void *workerRoutine(void *arg);
void initWorkerParams(WorkerThreadParams *params, ConnectionQueue *queue, int maxClientsPerThread, HttpRoutes *routes);
void initServerParams(HttpServerParams *params, short port, short numThreads, short maxClientsPerThread, HttpRoutes *routes);
int startHttpServer(HttpServerParams *params);
void parseHttpRequest(char *request, ParsedHttpRequest *parsedRequest);
HttpRoute *findHttpRoute(HttpRoutes *routes, char *path);
void defaultGETHandler(char *requestBody, char *response);
void errorResponse(char *response, int errorCode);
HttpRoute *createHttpRoute(char *name);
void addHttpSubroute(HttpRoute *parent, HttpRoute *child);
void initHttpRoutes(HttpRoutes *routes);


#endif // HTTPSERVER_H