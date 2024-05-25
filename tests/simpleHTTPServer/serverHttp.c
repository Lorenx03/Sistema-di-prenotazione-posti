#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main() {
    int sockfd, newsockfd;
    socklen_t clilen;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("ERROR opening socket");
    }
    printf("Socket created successfully.\n");

    // Bind socket to an address and port
    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR on binding");
    }

    printf("Binding completed.\n");

    // Listen for incoming connections
    listen(sockfd, 5);
    printf("Server listening on port %d...\n", PORT);
    clilen = sizeof(cli_addr);

    while (1) {
        // Accept a connection
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0) {
            error("ERROR on accept");
        }
        printf("Connection accepted.\n");

        // Read the request
        memset(buffer, 0, BUFFER_SIZE);
        n = read(newsockfd, buffer, BUFFER_SIZE - 1);
        if (n < 0) {
            error("ERROR reading from socket");
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
        n = write(newsockfd, response, strlen(response));
        if (n < 0) {
            error("ERROR writing to socket");
        }

        printf("Response sent.\n");

        // Close the connection
        close(newsockfd);
        printf("Connection closed.\n \n \n");

    }

    // Close the main socket
    close(sockfd);
    return 0;
}
