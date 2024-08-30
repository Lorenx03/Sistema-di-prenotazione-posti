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

// Struttura per la coda delle connessioni
typedef struct {
    int *sockets;
    int front;
    int rear;
    int count;
    int size;
    pthread_mutex_t lock;
    pthread_cond_t cond;
} ConnectionQueue;

// Parametri per il server HTTP
typedef struct {
    short port;
    short numThreads;
    short maxClientsPerThread;
} HttpServerParams;

// Parametri per i thread dei lavoratori
typedef struct {
    ConnectionQueue *queue;
    int maxClientsPerThread;
    int buffer_size;
} WorkerThreadParams;

// Dichiarazione delle funzioni
void initQueue(ConnectionQueue *queue, int size);
void enqueue(ConnectionQueue *queue, int socket);
int dequeue(ConnectionQueue *queue);
void destroyQueue(ConnectionQueue *queue);
void setNonBlocking(int socket);
void *workerRoutine(void *arg);
void initWorkerParams(WorkerThreadParams *params, ConnectionQueue *queue, int maxClientsPerThread, int buffer_size);
void initServerParams(HttpServerParams *params, short port, short numThreads, short maxClientsPerThread);
int startHttpServer(HttpServerParams *params);

#endif // HTTPSERVER_H