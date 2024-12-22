#include "httpServer.h"

// ================================ ROUTES ================================

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

HttpRoute *findHttpRoute(HttpRoute *currentNode, char *path) {
    if (path == NULL) return NULL;

    char pathCopy[MAX_ROUTE_NAME] = {0};
    strncpy(pathCopy, path, MAX_ROUTE_NAME - 1);
    pathCopy[MAX_ROUTE_NAME - 1] = '\0';

    char *saveptr;
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

// ================================ HTTP SERVER ================================

// ----------------------- RESPONSES -----------------------

void httpResponseBuilder(char *response, int statusCode, const char *statusMessage, const char *responseBody) {
    snprintf(response, MAX_RESPONSE_SIZE, HTTP_RESPONSE_TEMPLATE, statusCode, statusMessage, strlen(responseBody), responseBody);
}

void errorResponse(char *response, int errorCode) {
    const char *responseBody;
    const char *statusMessage;
    
    switch (errorCode) {
        case 400:
            responseBody = "Richiesta non valida";
            statusMessage = "Bad Request";
            break;
        case 404:
            responseBody = "Risorsa non trovata";
            statusMessage = "Not Found";
            break;
        case 405:
            responseBody = "Metodo non consentito";
            statusMessage = "Method Not Allowed";
            break;
        case 500:
            responseBody = "Errore interno del server";
            statusMessage = "Internal Server Error";
            break;
        default:
            responseBody = "Errore sconosciuto";
            statusMessage = "Unknown Error";
    }

    httpResponseBuilder(response, errorCode, statusMessage, responseBody);
}

// ----------------------- REQUESTS -----------------------

void parseHttpRequest(char *request, ParsedHttpRequest *parsedRequest) {
    // Initialize parsed request
    parsedRequest->method = UNKNOWN;
    parsedRequest->body = NULL;

    // Parse HTTP method
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

    // Find path
    char *path_start = strchr(request, ' ') + 1;
    char *path_end = strchr(path_start, ' ');
    if (path_end != NULL) {
        int path_length = path_end - path_start;
        strncpy(parsedRequest->path, path_start, path_length);
        parsedRequest->path[path_length] = '\0';
    }

    // Find request body
    char *body_start = strstr(request, "\r\n\r\n");
    if (body_start != NULL) {
        body_start += 4; // Skip \r\n\r\n
        parsedRequest->body = strdup(body_start);
    }

    // printf("Parsed request: %s %s\n", parsedRequest->path, parsedRequest->body);
}

// ----------------------- SOCKETS -----------------------

int sendall(int s, char *buf, int *len) {
    int total = 0;         
    int bytesleft = *len;  
    int n;

    while (total < *len) {
        n = send(s, buf + total, bytesleft, 0);
        if (n == -1) {
            break;
        }
        total += n;
        bytesleft -= n;
    }

    *len = total;             
    return n == -1 ? -1 : 0;
}

// ----------------------- CLIENTS -----------------------

int handleClient(int connSocketFd, HttpRoute *root){
    char rawRequest[MAX_REQUEST_SIZE] = {0};
    char response[MAX_RESPONSE_SIZE] = {0};

    // printf("\n\nHandling client\n");
    // READ
    ssize_t bytesRead = recv(connSocketFd, rawRequest, MAX_REQUEST_SIZE - 1, 0);
    if (bytesRead < 0) {
        fprintf(stderr, "Error reading from socket: %s\n", strerror(errno));
        return -1;
    }
    if (bytesRead == 0) {
        printf("Client disconnected\n");
        return -1;
    }
    rawRequest[bytesRead] = '\0';

    // printf("bytesRead: %zd\n", bytesRead);
    // printf("Raw request:\n%s\n", rawRequest);

    ParsedHttpRequest parsedRequest;
    parseHttpRequest(rawRequest, &parsedRequest);
    HttpRoute *route = findHttpRoute(root, parsedRequest.path);

    if (route != NULL) {
        if (route->handlers[parsedRequest.method] != NULL) {
            route->handlers[parsedRequest.method](parsedRequest.body, response);
        } else {
            errorResponse(response, 405);
        }
    } else {
        errorResponse(response, 404);
    }

    // WRITE
    int responseLength = strlen(response);
    sendall(connSocketFd, response, &responseLength);
    if (responseLength < 0) {
        fprintf(stderr, "Error sending response: %s\n", strerror(errno));
        return -1;
    }

    // printf("Response sent:\n%s\n", response);
    return 0;
}


// ----------------------- WORKER THREADS -----------------------

void *workerRoutine(void *params) {
    // Unpack params
    int id = ((WorkerThreadParams *)params)->id;
    int serverSocket = ((WorkerThreadParams *)params)->serverSocket;
    HttpRoute *root = ((WorkerThreadParams *)params)->root;

    // Client address
    struct sockaddr_in clientAddress;
    socklen_t clientLength;

    while (1) {
        // Accept
        int connSocketFd = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientLength);
        if (connSocketFd < 0) {
            fprintf(stderr, "Accept failed: %s\n", strerror(errno));
            continue;
        }

        // Handle
        handleClient(connSocketFd, root);

        // Close
        close(connSocketFd);
    }
}


// ----------------------- SERVER -----------------------

int httpServerServe(HttpServer *server) {
    int serverSocket; // Socket file descriptor
    struct sockaddr_in serverAddress; // Server address

    pthread_t threads[server->numThreads];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    // Bind socket to an address and port
    memset((char *)&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(server->port);

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        fprintf(stderr, "Socket creation failed: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    printf("Socket created successfully.\n");

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        fprintf(stderr, "Binding failed: %s\n", strerror(errno));
        close(serverSocket);
        return EXIT_FAILURE;
    }
    printf("Binding completed.\n");

    // Listen for incoming connections
    if (listen(serverSocket, SOMAXCONN) < 0) {
        fprintf(stderr, "Listening failed: %s\n", strerror(errno));
        close(serverSocket);
        return EXIT_FAILURE;
    }
    printf("Server listening on port %d...\n", server->port);

    // setNonBlocking(serverSocket);

    WorkerThreadParams workerParams[server->numThreads];
    for (int i = 0; i < server->numThreads; i++) {
        workerParams[i] = (WorkerThreadParams){
            .id = i,
            .serverSocket = serverSocket,
            .root = server->root,
        };
    }

    // Threadpool
    for (int i = 1; i < server->numThreads; i++) {
        if (pthread_create(&threads[i], &attr, workerRoutine, &workerParams[i]) != 0) {
            fprintf(stderr, "Thread creation failed: %s\n", strerror(errno));
            close(serverSocket);
            return EXIT_FAILURE;
        }
    }

    workerRoutine(&workerParams[0]);
    return 0;
}