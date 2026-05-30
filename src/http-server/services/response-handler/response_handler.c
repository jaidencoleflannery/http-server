#include <sys/socket.h>
#include <stdbool.h>

#include "./response_handler.h"

bool invoke_response(int socket_descriptor, char *message, char **response) {
    *response = "ok..."; 
    return true;
}

