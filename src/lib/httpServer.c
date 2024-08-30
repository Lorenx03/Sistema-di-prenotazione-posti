// This file contains the implementation of the HTTP server
#include "httpServer.h"

// ================== Socket queue data structure ================== //
void initQueue(ConnectionQueue *queue, int size) {
    queue->sockets = malloc(sizeof(int) * size);
    queue->front = 0;
    queue->rear = 0;
    queue->count = 0;
    queue->size = size;
    pthread_mutex_init(&queue->lock, NULL);
    pthread_cond_init(&queue->cond, NULL);
}

void enqueue(ConnectionQueue *queue, int socket) {
    pthread_mutex_lock(&queue->lock);
    while (queue->count == queue->size) {
        pthread_cond_wait(&queue->cond, &queue->lock);
    }
    queue->sockets[queue->rear] = socket;
    queue->rear = (queue->rear + 1) % queue->size;
    queue->count++;
    pthread_cond_signal(&queue->cond);
    pthread_mutex_unlock(&queue->lock);
}

int dequeue(ConnectionQueue *queue) {
    pthread_mutex_lock(&queue->lock);
    while (queue->count == 0) {
        pthread_cond_wait(&queue->cond, &queue->lock);
    }
    int socket = queue->sockets[queue->front];
    queue->front = (queue->front + 1) % queue->size;
    queue->count--;
    pthread_cond_signal(&queue->cond);
    pthread_mutex_unlock(&queue->lock);
    return socket;
}

void destroyQueue(ConnectionQueue *queue) {
    free(queue->sockets);
    pthread_mutex_destroy(&queue->lock);
    pthread_cond_destroy(&queue->cond);
}


// ================== Worker thread routine ================== //
void setNonBlocking(int socket) {
    int flags = fcntl(socket, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl F_GETFL failed");
        exit(EXIT_FAILURE);
    }
    if (fcntl(socket, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl F_SETFL failed");
        exit(EXIT_FAILURE);
    }
}

void *workerRoutine(void *arg) {
    WorkerThreadParams *params = (WorkerThreadParams *)arg;
    struct pollfd pfds[params->maxClientsPerThread];
    char buffers[params->maxClientsPerThread][params->buffer_size];
    int client_count = 0;

    while (1) {
        // If there's room for more clients, dequeue and add to pfds array
        if (client_count < params->maxClientsPerThread) {
            int connectionSocket = dequeue(params->queue);
            setNonBlocking(connectionSocket);

            pfds[client_count].fd = connectionSocket;
            pfds[client_count].events = POLLIN;
            client_count++;
        }

        // Poll the sockets
        int poll_count = poll(pfds, client_count, -1);

        if (poll_count < 0) {
            perror("poll error");
            for (int i = 0; i < client_count; i++) {
                close(pfds[i].fd);
            }
            continue;
        }

        // Iterate over the clients to check for events
        for (int i = 0; i < client_count; i++) {
            if (pfds[i].revents & POLLIN) {
                memset(buffers[i], 0, params->buffer_size);
                int n = read(pfds[i].fd, buffers[i], params->buffer_size - 1);
                if (n < 0) {
                    perror("Error reading from socket");
                    close(pfds[i].fd);
                    pfds[i].fd = -1;
                } else if (n == 0) {
                    // Connection closed by client
                    close(pfds[i].fd);
                    pfds[i].fd = -1;
                } else {
                    // Process the request
                    printf("Here is the request from client %d:\n%s\n", i, buffers[i]);

                    const char *response_body = "Ciaoooooo";
                    const char *response_header_template =
                        "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/plain\r\n"
                        "Content-Length: %zu\r\n"
                        "\r\n"
                        "%s";

                    char response[params->buffer_size];
                    snprintf(response, params->buffer_size, response_header_template, strlen(response_body), response_body);
                    printf("Response:\n%s \n\n", response);

                    n = write(pfds[i].fd, response, strlen(response));
                    if (n < 0) {
                        perror("Error writing to socket");
                    }

                    printf("Response sent.\n");

                    close(pfds[i].fd);
                    pfds[i].fd = -1;
                }
            }
        }

        // Remove closed connections from the poll array
        int j = 0;
        for (int i = 0; i < client_count; i++) {
            if (pfds[i].fd != -1) {
                pfds[j++] = pfds[i];
            }
        }
        client_count = j;
    }

    return NULL;
}

void initWorkerParams(WorkerThreadParams *params, ConnectionQueue *queue, int maxClientsPerThread, int buffer_size) {
    params->queue = queue;
    params->maxClientsPerThread = maxClientsPerThread;
    params->buffer_size = buffer_size;
}


// ================== HTTP server ================== //

void initServerParams(HttpServerParams *params, short port, short numThreads, short maxClientsPerThread) {
    params->port = port;
    params->numThreads = numThreads;
    params->maxClientsPerThread = maxClientsPerThread;
}

int startHttpServer(HttpServerParams *params) {
    int serverSocket; // Socket file descriptor
    int connectionSocket; // Socket file descriptor
    socklen_t clientLength; // Client address length
    struct sockaddr_in serverAddress; // Server address
    struct sockaddr_in clientAddress; // Client address
    
    pthread_t threads[params->numThreads];
    pthread_attr_t attr;

    ConnectionQueue queue;
    initQueue(&queue, params->numThreads * params->maxClientsPerThread);

    WorkerThreadParams workerParams;
    initWorkerParams(&workerParams, &queue, params->maxClientsPerThread, 1024);

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Socket creation failed");
        return EXIT_FAILURE;
    }
    printf("Socket created successfully.\n");

    // Bind socket to an address and port
    memset((char *)&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(params->port);

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Binding failed");
        close(serverSocket);
        return EXIT_FAILURE;
    }
    printf("Binding completed.\n");

    // Listen for incoming connections
    if (listen(serverSocket, 5) < 0) {
        perror("Listening failed");
        close(serverSocket);
        return EXIT_FAILURE;
    }
    printf("Server listening on port %d...\n", params->port);

    
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    for (int i = 0; i < params->numThreads; i++) {
        if (pthread_create(&threads[i], &attr, workerRoutine, &workerParams) != 0) {
            perror("Thread creation failed");
            close(serverSocket);
            return EXIT_FAILURE;
        }
    }

    clientLength = sizeof(clientAddress);

    while (1) {
        connectionSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientLength);
        if (connectionSocket < 0) {
            perror("Accept failed");
            close(serverSocket);
            return EXIT_FAILURE;
        }
        printf("Connection accepted.\n");

        enqueue(&queue, connectionSocket);
    }

    close(serverSocket);
    pthread_attr_destroy(&attr);
    destroyQueue(&queue);
    return 0;
}