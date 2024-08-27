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

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_THREADS 4
#define QUEUE_SIZE 10

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

void *workerRoutine(void *arg) {
    struct pollfd pfd;
    char buffer[BUFFER_SIZE];

    while (1) {
        int connectionSocket = dequeue(&queue);

        pfd.fd = connectionSocket;
        pfd.events = POLLIN;

        int poll_count = poll(&pfd, 1, -1);

        if (poll_count < 0) {
            perror("poll error");
            close(connectionSocket);
            continue;
        }

        if (pfd.revents & POLLIN) {
            memset(buffer, 0, BUFFER_SIZE);
            int n = read(connectionSocket, buffer, BUFFER_SIZE - 1);
            if (n < 0) {
                perror("Error reading from socket");
                close(connectionSocket);
                continue;
            }

            printf("Here is the request:\n%s\n", buffer);

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

            n = write(connectionSocket, response, strlen(response));
            if (n < 0) {
                perror("Error writing to socket");
            }

            printf("Response sent.\n");

            close(connectionSocket);
            printf("Connection closed.\n \n \n");
        }
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