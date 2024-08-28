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

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_THREADS 4
#define QUEUE_SIZE 10
#define MAX_CLIENTS_PER_THREAD 10

typedef struct {
    int *sockets;
    int front;
    int rear;
    int count;
    pthread_mutex_t lock;
    pthread_cond_t cond;
} ConnectionQueue;

int status = 0;
ConnectionQueue queue;

void initQueue(ConnectionQueue *queue, int size) {
    queue->sockets = malloc(sizeof(int) * size);
    queue->front = 0;
    queue->rear = 0;
    queue->count = 0;
    pthread_mutex_init(&queue->lock, NULL);
    pthread_cond_init(&queue->cond, NULL);
}

void enqueue(ConnectionQueue *queue, int socket) {
    pthread_mutex_lock(&queue->lock);
    while (queue->count == QUEUE_SIZE) {
        pthread_cond_wait(&queue->cond, &queue->lock);
    }
    queue->sockets[queue->rear] = socket;
    queue->rear = (queue->rear + 1) % QUEUE_SIZE;
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
    queue->front = (queue->front + 1) % QUEUE_SIZE;
    queue->count--;
    pthread_cond_signal(&queue->cond);
    pthread_mutex_unlock(&queue->lock);
    return socket;
}

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
    struct pollfd pfds[MAX_CLIENTS_PER_THREAD];
    char buffers[MAX_CLIENTS_PER_THREAD][BUFFER_SIZE];
    int client_count = 0;

    while (1) {
        // If there's room for more clients, dequeue and add to pfds array
        if (client_count < MAX_CLIENTS_PER_THREAD) {
            int connectionSocket = dequeue(&queue);
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

                    const char *response_body = "Ciaoooooo";
                    const char *response_header_template =
                        "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/plain\r\n"
                        "Content-Length: %zu\r\n"
                        "\r\n"
                        "%s";

                    char response[BUFFER_SIZE];
                    snprintf(response, BUFFER_SIZE, response_header_template, strlen(response_body), response_body);
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

    pthread_exit((void *)&status);
}

int main(int argc, char *argv[]) {
    int serverSocket; // Socket file descriptor
    int connectionSocket; // Socket file descriptor
    socklen_t clientLength; // Client address length
    struct sockaddr_in serverAddress; // Server address
    struct sockaddr_in clientAddress; // Client address

    initQueue(&queue, QUEUE_SIZE);

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket created successfully.\n");

    // Bind socket to an address and port
    memset((char *)&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Binding failed");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }
    printf("Binding completed.\n");

    // Listen for incoming connections
    if (listen(serverSocket, 5) < 0) {
        perror("Listening failed");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port %d...\n", PORT);

    pthread_t threads[MAX_THREADS];
    for (int i = 0; i < MAX_THREADS; i++) {
        if (pthread_create(&threads[i], NULL, workerRoutine, NULL) != 0) {
            perror("Thread creation failed");
            close(serverSocket);
            exit(EXIT_FAILURE);
        }
    }

    clientLength = sizeof(clientAddress);

    while (1) {
        connectionSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientLength);
        if (connectionSocket < 0) {
            perror("Accept failed");
            close(serverSocket);
            exit(EXIT_FAILURE);
        }
        printf("Connection accepted.\n");

        enqueue(&queue, connectionSocket);
    }

    close(serverSocket);
    return 0;
}
