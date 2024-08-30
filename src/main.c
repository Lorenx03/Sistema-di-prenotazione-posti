#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "lib/linkedList.h"
#include "lib/userInput.h"
#include "lib/gui.h"
#include "lib/httpServer.h"

int main() {
    // Initialize the server
    HttpServerParams params;
    initServerParams(&params, 8080, 4, 10);
    startHttpServer(&params);
}