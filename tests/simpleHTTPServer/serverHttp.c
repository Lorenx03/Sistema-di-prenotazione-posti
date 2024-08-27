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

#define PORT 8080
#define BUFFER_SIZE 1024

int status = 0;

void *threadRoutine(void *arg){
    printf("Thread created\n");
    int connectionSocket = *((int *)arg);
    int n = 0;
    char buffer[BUFFER_SIZE];

    // Read the request
    memset(buffer, 0, BUFFER_SIZE);
    n = read(connectionSocket, buffer, BUFFER_SIZE - 1);
    if (n < 0) {
        fprintf(stderr, "Error on reading\n");
        exit(EXIT_FAILURE);
    }

    // Print the request to stdout
    printf("Here is the request:\n%s\n", buffer);

    // Send the response
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

    // send the response
    n = write(connectionSocket, response, strlen(response));
    if (n < 0) {
        fprintf(stderr, "Error on writing\n");
        exit(EXIT_FAILURE);
    }

    printf("Response sent.\n");

    // Close the connection
    close(connectionSocket);
    printf("Connection closed.\n \n \n");

    free(arg);
    pthread_exit((void *)&status);
}






int main(int argc, char *argv[]) {
    int serverSocket; // Socket file descriptor
    int connectionSocket; // Socket file descriptor

    socklen_t clientLength; // Client address length
    struct sockaddr_in serverAddress; // Server address
    struct sockaddr_in clientAddress; // Client address

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        fprintf(stderr, "Err");
		exit(EXIT_FAILURE);
    }
    printf("Socket created successfully.\n");



    // Bind socket to an address and port
    memset((char *)&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET; // Address family
    serverAddress.sin_addr.s_addr = INADDR_ANY; 
    serverAddress.sin_port = htons(PORT); // little endian to big endian



    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        fprintf(stderr, "Error on binding\n");
		exit(EXIT_FAILURE);
    }
    printf("Binding completed.\n");



    // Listen for incoming connections
    listen(serverSocket, 5);
    printf("Server listening on port %d...\n", PORT);
    clientLength = sizeof(clientAddress);



    pthread_t tid;
    while (1) {
        // Accept a connection
        connectionSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientLength);
        if (connectionSocket < 0) {
            fprintf(stderr, "Error on accept\n");
		    exit(EXIT_FAILURE);
        }
        printf("Connection accepted.\n");

        int *threadConnectionSocket = malloc(sizeof(int));
        if (threadConnectionSocket == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        *threadConnectionSocket = connectionSocket;

        // Create a new thread to handle the connection
        if(pthread_create(&tid, NULL, threadRoutine, threadConnectionSocket) != 0){
            fprintf(stderr, "Error on creating thread\n");
            exit(EXIT_FAILURE);
        }
    }

    // Close the main socket
    close(serverSocket);
    return 0;
}
