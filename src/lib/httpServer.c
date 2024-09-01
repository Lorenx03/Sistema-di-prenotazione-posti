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
    char buffers[params->maxClientsPerThread][BUFFER_SIZE];
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
                memset(buffers[i], 0, BUFFER_SIZE);
                int n = read(pfds[i].fd, buffers[i], BUFFER_SIZE - 1);
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

                    ParsedHttpRequest parsedRequest;
                    parseHttpRequest(buffers[i], &parsedRequest);
                    char response[BUFFER_SIZE];

                    HttpRoute *route = findHttpRoute(params->routes, parsedRequest.path);
                    if (route != NULL) {
                        if (route->handlers[parsedRequest.method] != NULL) {
                            route->handlers[parsedRequest.method](parsedRequest.body, response);
                        } else {
                            errorResponse(response, 405);
                        }
                    } else {
                        errorResponse(response, 404);
                    }

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

void initWorkerParams(WorkerThreadParams *params, ConnectionQueue *queue, int maxClientsPerThread, HttpRoutes *routes) {
    params->queue = queue;
    params->maxClientsPerThread = maxClientsPerThread;
    params->routes = routes;
}


void parseHttpRequest(char *request, ParsedHttpRequest *parsedRequest) {
    // Inizializza la struttura ParsedHttpRequest
    parsedRequest->method = UNKNOWN;
    parsedRequest->body = NULL;

    // Analizza il metodo HTTP
    if (strncmp(request, "GET", 3) == 0) {
        parsedRequest->method = GET;
    } else if (strncmp(request, "POST", 4) == 0) {
        parsedRequest->method = POST;
    } else if (strncmp(request, "PUT", 3) == 0) {
        parsedRequest->method = PUT;
    } else if (strncmp(request, "DELETE", 6) == 0) {
        parsedRequest->method = DELETE;
    } else if (strncmp(request, "HEAD", 4) == 0) {
        parsedRequest->method = HEAD;
    } else if (strncmp(request, "OPTIONS", 7) == 0) {
        parsedRequest->method = OPTIONS;
    } else if (strncmp(request, "TRACE", 5) == 0) {
        parsedRequest->method = TRACE;
    } else if (strncmp(request, "CONNECT", 7) == 0) {
        parsedRequest->method = CONNECT;
    } else if (strncmp(request, "PATCH", 5) == 0) {
        parsedRequest->method = PATCH;
    } else {
        parsedRequest->method = UNKNOWN;
    }

    // Trova il percorso
    char *path_start = strchr(request, ' ') + 1;
    char *path_end = strchr(path_start, ' ');
    if (path_end != NULL) {
        int path_length = path_end - path_start;
        strncpy(parsedRequest->path, path_start, path_length);
        parsedRequest->path[path_length] = '\0';
    }

    // Trova il corpo della richiesta (se presente)
    char *body_start = strstr(request, "\r\n\r\n");
    if (body_start != NULL) {
        body_start += 4; // Salta i caratteri \r\n\r\n
        parsedRequest->body = strdup(body_start);
    }

    printf("Parsed request: %s %s\n", parsedRequest->path, parsedRequest->body);
}


// ================== HTTP server ================== //

void initServerParams(HttpServerParams *params, short port, short numThreads, short maxClientsPerThread, HttpRoutes *routes) {
    params->port = port;
    params->numThreads = numThreads;
    params->maxClientsPerThread = maxClientsPerThread;
    params->routes = routes;
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
    initWorkerParams(&workerParams, &queue, params->maxClientsPerThread, params->routes);

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


// ================== HTTP routes ================== //

void defaultGETHandler(char *requestBody, char *response) {
    const char *response_body = "Ciaoooooo";
    const char *response_header_template =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: %zu\r\n"
        "\r\n"
        "%s";

    snprintf(response, 1024, response_header_template, strlen(response_body), response_body);
}


void errorResponse(char *response, int errorCode) {
    const char *response_body;
    const char *status_message;
    
    switch (errorCode) {
        case 400:
            response_body = "Richiesta non valida";
            status_message = "Bad Request";
            break;
        case 404:
            response_body = "Risorsa non trovata";
            status_message = "Not Found";
            break;
        case 405:
            response_body = "Metodo non consentito";
            status_message = "Method Not Allowed";
            break;
        case 500:
            response_body = "Errore interno del server";
            status_message = "Internal Server Error";
            break;
        default:
            response_body = "Errore sconosciuto";
            status_message = "Unknown Error";
    }

    const char *response_header_template =
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: %zu\r\n"
        "Server: MioServer/1.0\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s";

    snprintf(response, BUFFER_SIZE, response_header_template, errorCode, status_message, strlen(response_body), response_body);
}
        



HttpRoute *createHttpRoute(char *name) {
    HttpRoute *route = malloc(sizeof(HttpRoute));
    
    if(strlen(name) > MAX_ROUTE_NAME) {
        perror("Route name too long");
        exit(EXIT_FAILURE);
    }

    strcpy(route->name, name);
    route->parent = NULL;
    route->sibling = NULL;
    route->child = NULL;

    route->handlers[GET] = defaultGETHandler;

    for (int i = 1; i < 9; i++) {
        route->handlers[i] = NULL;
    }

    return route;
}


void initHttpRoutes(HttpRoutes *routes) {
    routes->root = createHttpRoute("/");
}


void addHttpSubroute(HttpRoute *subTreeRoot, HttpRoute *newChild) {
    newChild->parent = subTreeRoot;

    if (subTreeRoot->child == NULL){
        subTreeRoot->child = newChild;
    }else{
        HttpRoute *currentNode = subTreeRoot->child;
        while (currentNode->sibling != NULL) {
            currentNode = currentNode->sibling;
        }
        currentNode->sibling = newChild;
    }
}


HttpRoute *findHttpRoute(HttpRoutes *routes, char *path) {
    HttpRoute *currentNode = routes->root;
    char *saveptr;
    char pathCopy[MAX_ROUTE_NAME];
    strcpy(pathCopy, path);
    char *token = strtok_r(pathCopy, "/", &saveptr);
    
    while (token != NULL) {
        HttpRoute *child = currentNode->child;
        while (child != NULL) {
            if (strcmp(child->name, token) == 0) {
                currentNode = child;
                break;
            }
            child = child->sibling;
        }
        if (child == NULL) {
            return NULL;
        }
        token = strtok_r(NULL, "/", &saveptr);
    }
    return currentNode;
}


// navigate the tree to find the correct place to insert the new route
// void addHttpRoute(HttpRoutes *routes, HttpRoute *newRoute, char *path) {
//     HttpRoute *currentNode = routes->root;
//     char *token = strtok(path, "/");
//     while (token != NULL) {
//         HttpRoute *child = currentNode->child;
//         while (child != NULL) {
//             if (strcmp(child->name, token) == 0) {
//                 currentNode = child;
//                 break;
//             }
//             child = child->sibling;
//         }

//         if (child == NULL) {
//             HttpRoute *newNode = createHttpRoute(token);
//             addHttpSubroute(currentNode, newNode);
//             currentNode = newNode;
//         }

//         token = strtok(NULL, "/");
//     }
// }