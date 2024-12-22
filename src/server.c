#include <stdio.h>
#include "httpServer.h"

void GETrootHandler(char *request, char *response) {
    char response_body[100];
    snprintf(response_body, sizeof(response_body), "Root - Thread ID: %ld", (long)pthread_self());
    httpResponseBuilder(response, 200, "OK", response_body);
}

void GEThelloHandler(char *request, char *response) {
    char response_body[100];
    snprintf(response_body, sizeof(response_body), "Hello - Thread ID: %ld", (long)pthread_self());
    httpResponseBuilder(response, 200, "OK", response_body);
}

int main() {
    HttpRoute root = {0};
    root.name = "/";
    root.handlers[GET] = GETrootHandler;

    HttpRoute hello = {0};
    hello.name = "hello";
    hello.handlers[GET] = GEThelloHandler;

    addHttpSubroute(&root, &hello);

    HttpServer server = {
        .port = 8090,
        .numThreads = 4,
        .root = &root
    };

    if (httpServerServe(&server)) {
        fprintf(stderr, "Error starting server\n");
        return 1;
    }
    return 0;
}