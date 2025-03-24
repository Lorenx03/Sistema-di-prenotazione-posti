#include "httpServer.h"

atomic_int running = 1; // Flag to stop the server

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
    snprintf(response, MAX_RESPONSE_SIZE, HTTP_RESPONSE_TEMPLATE, statusCode, statusMessage, "text/plain", strlen(responseBody), responseBody);
}

void httpResponseBuilderHTML(char *response, int statusCode, const char *statusMessage, const char *responseBody) {
    snprintf(response, MAX_RESPONSE_SIZE, HTTP_RESPONSE_TEMPLATE, statusCode, statusMessage, "text/html", strlen(responseBody), responseBody);
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
    memset(parsedRequest->body, 0, MAX_REQUEST_SIZE);

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
        // parsedRequest->body = strdup(body_start);

        if (strlen(body_start) < MAX_REQUEST_SIZE) {
            strncpy(parsedRequest->body, body_start, MAX_REQUEST_SIZE - 1);
            parsedRequest->body[MAX_REQUEST_SIZE - 1] = '\0';
        }
    }

    // printf("Parsed request: %s %s\n", parsedRequest->path, parsedRequest->body);
}

// ----------------------- SOCKETS -----------------------

int sendall(int s, char *buf, int *len) {
    int total = 0;         
    int bytesleft = *len;  
    int n = 0;

    while (total < *len) {
        n = send(s, buf + total, bytesleft, 0);
        if (n == -1) {
            if (errno == EPIPE) {
                fprintf(stderr, "Client connection closed unexpectedly\n");
            }
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

    struct timeval timeout;
    timeout.tv_sec = 10;  // 10s
    timeout.tv_usec = 0;
    if (setsockopt(connSocketFd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        fprintf(stderr, "handleClient: setsockopt timeout failed: %s\n", strerror(errno));
        return -1;
    }
    if (setsockopt(connSocketFd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
        fprintf(stderr, "handleClient: setsockopt timeout failed: %s\n", strerror(errno));
        return -1;
    }

    // READ
    ssize_t bytesRead = 0;
    size_t totalBytesRead = 0;
    bool requestComplete = false;
    
    while (!requestComplete && totalBytesRead < MAX_REQUEST_SIZE - 1) {
        bytesRead = recv(connSocketFd, rawRequest + totalBytesRead, MAX_REQUEST_SIZE - totalBytesRead - 1, 0);
        
        if (bytesRead < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // Timeout
                fprintf(stderr, "handleClient: timeout waiting for data from client\n");
            } else {
                fprintf(stderr, "Error reading from socket: %s\n", strerror(errno));
            }
            return -1;
        }
        
        if (bytesRead == 0) {
            printf("Client disconnected\n");
            return -1;
        }
        
        totalBytesRead += bytesRead;
        rawRequest[totalBytesRead] = '\0';
        
        // Verify if the request is complete
        char *headerEnd = strstr(rawRequest, "\r\n\r\n");
        if (headerEnd != NULL) {
            headerEnd += 4; // Skip "\r\n\r\n"
            
            char *contentLength = strstr(rawRequest, "Content-Length:");
            if (contentLength != NULL) {
                // Extract the content length as a string first
                char lengthStr[32] = {0};
                char *start = contentLength + 15; // Skip "Content-Length: "
                char *end = strpbrk(start, "\r\n"); // Find end of header line
                
                if (end != NULL) {
                    size_t len = end - start;
                    if (len < sizeof(lengthStr)) {
                        strncpy(lengthStr, start, len);
                        lengthStr[len] = '\0';
                        
                        int length = safeStrToInt(lengthStr);
                        // printf("Content-Length: %d\n", length);

                        if (length >= 0) {
                            size_t bodyReceived = totalBytesRead - (headerEnd - rawRequest);
                            
                            // If we have received the entire body, the request is complete
                            if (bodyReceived >= (size_t)length) {
                                requestComplete = true;
                            }
                        } else {
                            fprintf(stderr, "Invalid Content-Length value\n");
                            requestComplete = true; 
                        }
                    } else {
                        fprintf(stderr, "Content-Length value too long\n");
                        requestComplete = true;
                    }
                } else {
                    fprintf(stderr, "Malformed Content-Length header\n");
                    requestComplete = true;
                }
            } else {
                // No Content-Length, the request is complete after the headers
                requestComplete = true;
            }
        }
    }
    
    if (!requestComplete && totalBytesRead >= MAX_REQUEST_SIZE - 1) {
        fprintf(stderr, "Request too large or incomplete, truncated\n");
        errorResponse(response, HTTP_STATUS_INTERNAL_SERVER_ERROR);
    }else{
        ParsedHttpRequest parsedRequest;
        parseHttpRequest(rawRequest, &parsedRequest);
        HttpRoute *route = findHttpRoute(root, parsedRequest.path);

        if (route != NULL) {
            if (route->handlers[parsedRequest.method] != NULL) {
                route->handlers[parsedRequest.method](parsedRequest.body, response); // Call the handler
            } else {
                errorResponse(response, HTTP_STATUS_METHOD_NOT_ALLOWED);
            }
        } else {
            errorResponse(response, HTTP_STATUS_NOT_FOUND);
        }
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
    // int id = ((WorkerThreadParams *)params)->id;
    int serverSocket = ((WorkerThreadParams *)params)->serverSocket;
    HttpRoute *root = ((WorkerThreadParams *)params)->root;

    // Client address
    struct sockaddr_in clientAddress;
    socklen_t clientLength;

    while (atomic_load(&running) == 1) {
        // Accept
        int connSocketFd = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientLength);
        if (connSocketFd < 0 && atomic_load(&running) == 1) {
            fprintf(stderr, "Accept failed: %s\n", strerror(errno));
            continue;
        }else if (running <= 0){
            // "Error" caused by the server stopping
            break;
        }

        // Handle
        handleClient(connSocketFd, root);

        // Close
        close(connSocketFd);
    }

    return NULL;
}

// ----------------------- CRON JOBS -----------------------

void runJob(CronJob *job) {
    while (running == 1) {
        job->job();
        sleep(job->interval);
    }
    job->job(); // run one last time before exiting
}

void runCronJobs(void *params) {
    HttpServerCronJobs *jobs = (HttpServerCronJobs*)params;

    pthread_t threads[jobs->numJobs];       
    for (int i = 0; i < jobs->numJobs; i++) {
        pthread_create(&threads[i], NULL, (void*)runJob, &jobs->jobs[i]);
    }

    for (int i = 0; i < jobs->numJobs; i++) {
        pthread_join(threads[i], NULL);
    }
}

int printHostInfo() {
    char hostname[256];
    struct addrinfo hints, *info, *p;
    int ret;

    // Get hostname
    if (gethostname(hostname, sizeof(hostname)) == -1) {
        perror("gethostname");
        return EXIT_FAILURE;
    }

    printf("\n========= HOST INFO ========\n");
    printf("Hostname: %s\n", hostname);

    // Resolve IP address
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4 only
    hints.ai_socktype = SOCK_STREAM;

    if ((ret = getaddrinfo(hostname, NULL, &hints, &info)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
        return EXIT_FAILURE;
    }

    // Print all resolved addresses
    for (p = info; p != NULL; p = p->ai_next) {
        struct sockaddr_in *addr = (struct sockaddr_in *)p->ai_addr;
        printf("IPv4 Address: %s\n", inet_ntoa(addr->sin_addr));
    }

    freeaddrinfo(info);
    printf("============================\n\n");
    return EXIT_SUCCESS;
}


// ----------------------- SERVER -----------------------

void handleSigStop(int sig) {
    (void)sig;
    printf("Stopping server...\n");
    atomic_fetch_sub(&running, 1);

    if (atomic_load(&running) <= -2){
        fprintf(stderr, "Server stopped forcefully\n");
        exit(EXIT_FAILURE);
    }
}

void handleCriticalError(int sig) {
    const char *signame;
    
    switch (sig) {
        case SIGSEGV: signame = "SIGSEGV (Segmentation Fault)"; break;
        case SIGBUS:  signame = "SIGBUS (Bus Error)"; break;
        case SIGFPE:  signame = "SIGFPE (Floating Point Exception)"; break;
        case SIGILL:  signame = "SIGILL (Illegal Instruction)"; break;
        case SIGABRT: signame = "SIGABRT (Abort)"; break;
        default:      signame = "Unknown"; break;
    }
    
    fprintf(stderr, "\n======== CRITICAL ERROR ========\n");
    fprintf(stderr, "Received signal: %s (number: %d)\n", signame, sig);
    fprintf(stderr, "Attempting safe shutdown...\n");
    atomic_store(&running, -10);
    
    fprintf(stderr, "The server will be terminated.\n");
    fprintf(stderr, "================================\n\n");
    
    
    struct sigaction sa_dfl;
    sa_dfl.sa_handler = SIG_DFL;
    sigemptyset(&sa_dfl.sa_mask);
    sa_dfl.sa_flags = 0;
    sigaction(sig, &sa_dfl, NULL);

    raise(sig);
}

int httpServerServe(HttpServer *server) {
    // Signal handlers
    struct sigaction sa_stop;
    sa_stop.sa_handler = handleSigStop;  
    sa_stop.sa_flags = 0;                
    sigfillset(&sa_stop.sa_mask);

    sigaction(SIGINT, &sa_stop, NULL);
    sigaction(SIGQUIT, &sa_stop, NULL);
    sigaction(SIGTERM, &sa_stop, NULL);
    sigaction(SIGHUP, &sa_stop, NULL);

    struct sigaction sa_ignore;
    sa_ignore.sa_handler = SIG_IGN;
    sigemptyset(&sa_ignore.sa_mask);
    sa_ignore.sa_flags = 0;
    
    sigaction(SIGPIPE, &sa_ignore, NULL);
    sigaction(SIGALRM, &sa_ignore, NULL);
    sigaction(SIGUSR1, &sa_ignore, NULL); 
    sigaction(SIGUSR2, &sa_ignore, NULL);

    struct sigaction sa_critical;
    sa_critical.sa_handler = handleCriticalError;
    sigfillset(&sa_critical.sa_mask);
    sa_critical.sa_flags = 0;
    
    sigaction(SIGSEGV, &sa_critical, NULL);
    sigaction(SIGBUS, &sa_critical, NULL);
    sigaction(SIGFPE, &sa_critical, NULL);
    sigaction(SIGILL, &sa_critical, NULL);
    sigaction(SIGABRT, &sa_critical, NULL);


    int serverSocket; // Socket file descriptor
    struct sockaddr_in serverAddress; // Server address

    pthread_t threads[server->numThreads]; // Worker threads
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

    // Reuse port
    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        fprintf(stderr, "Setting socket options failed: %s\n", strerror(errno));
        close(serverSocket);
        return EXIT_FAILURE;
    }

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

    printHostInfo();
    printf("Server listening on port %d...\n", server->port);


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
        if (pthread_create(&threads[i-1], &attr, workerRoutine, &workerParams[i]) != 0) {
            fprintf(stderr, "Thread creation failed: %s\n", strerror(errno));
            close(serverSocket);
            return EXIT_FAILURE;
        }
    }


    // Cron jobs
    pthread_t cronThread;
    if(server->cronJobs != NULL){
        if (server->cronJobs->numJobs > 0) {
            if (pthread_create(&cronThread, NULL, (void*)runCronJobs, server->cronJobs) != 0) {
                fprintf(stderr, "Cron thread creation failed: %s\n", strerror(errno));
                close(serverSocket);
                return EXIT_FAILURE;
            }
        }
    }

    workerRoutine(&workerParams[0]);

    // Cleanup
    if(server->cronJobs != NULL){
        if (server->cronJobs->numJobs > 0) {
            pthread_join(cronThread, NULL);
        }
    }

    shutdown(serverSocket, SHUT_RDWR);
    close(serverSocket);

    pthread_attr_destroy(&attr);

    printf("Server stopped - (press any key to continue)\n");
    waitForKey();
    return 0;
}