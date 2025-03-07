#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "lib/linkedList.h"
#include "lib/userInput.h"
#include "lib/gui.h"
#include "lib/httpServer.h"

void helloHandler(char *body, char *response) {
    const char *response_body = "Ciaoooooo from helloHandler!";
    const char *response_header_template =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: %zu\r\n"
        "\r\n"
        "%s";

    snprintf(response, 1024, response_header_template, strlen(response_body), response_body);
}

void echoHandler(char *body, char *response) {
    const char *response_body = body;
    const char *response_header_template =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: %zu\r\n"
        "\r\n"
        "%s";

    snprintf(response, 1024, response_header_template, strlen(response_body), response_body);
}

int main() {
    // Inizializza le route
    HttpRoutes routes;
    initHttpRoutes(&routes);

    // Aggiungi alcune route di test
    HttpRoute *helloRoute = createHttpRoute("hello");
    helloRoute->handlers[GET] = helloHandler;
    addHttpSubroute(routes.root, helloRoute);

    HttpRoute *echoRoute = createHttpRoute("echo");
    echoRoute->handlers[POST] = echoHandler;
    addHttpSubroute(routes.root, echoRoute);

    // Inizializza il server
    HttpServerParams params;
    initServerParams(&params, 8080, 4, 10, &routes);
    startHttpServer(&params);

    return 0;
}