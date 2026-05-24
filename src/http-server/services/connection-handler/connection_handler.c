#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "types/address-types/address_types.h"
#include "services/logging/logging.h"
#include "configuration/configuration-handler/configuration_handler.h"

#include "./connection_handler.h"

bool find_bind(addrinfo *addresses, int **socket_descriptor) {
    const size_t port = config.port;
    const size_t max_connections = config.max_connections;

    while(addresses) { 
        // if debugging use localhost loopback.
        #ifndef NDEBUG
            sockaddr_in address = (sockaddr_in){
                .sin_family = AF_INET,
                .sin_port = htons(port)
            };
            inet_pton(AF_INET, IPV4_LOCALHOST, &address.sin_addr);
        #else
            sockaddr *address = addresses->ai_addr;
        #endif 

        int testing_descriptor;
        if((testing_descriptor = socket(addresses->ai_family, addresses->ai_protocol, 0)) < 0) {
            char *socket_error = strerror(errno);
            DEBUG_LOG("find_bind: Failed to open a socket, error: %s.\n", socket_error);
            addresses = addresses->ai_next; 
            continue;
        }

        setsockopt(testing_descriptor, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));

        if(bind(testing_descriptor, addresses->ai_addr, addresses->ai_addrlen) < 0) {
            char *bind_error = strerror(errno);
            DEBUG_LOG("find_bind: Failed to bind to local port, error: %s.\n", bind_error);
            close(testing_descriptor);
            addresses = addresses->ai_next; 
            continue;
        } else {
            *socket_descriptor = memcpy(malloc(sizeof(testing_descriptor)), &testing_descriptor, sizeof(testing_descriptor));
            break;
        }
    }

    if(*socket_descriptor == NULL) {
        DEBUG_LOG("find_bind: No successful bindings for provided addresses.");
        return false;
    }

    return true;
}

bool find_connection(addrinfo *addresses) {
    // successful connect fd.
    int *socket_descriptor = NULL;

    // iterate, first fit algo.  
    if(!find_bind(addresses, &socket_descriptor)) {
        DEBUG_LOG("find_connection: Failed to find to any provided addresses.");
        return false;
    }

    if(connect(*socket_descriptor, addresses->ai_addr, (unsigned int)addresses->ai_addrlen) < 0) {
        char *connect_error = strerror(errno);
        DEBUG_LOG("find_connection: Failed to bind to local port, error: %s.\n", connect_error);
        close(*socket_descriptor);
    }

    return true;
}

bool find_listen(addrinfo *addresses) {
    // successful connect fd.
    int *socket_descriptor = NULL;

    // iterate, first fit algo.  
    if(!find_bind(addresses, &socket_descriptor)) {
        DEBUG_LOG("find_listen: Failed to find to any provided addresses.");
        return false;
    }

    struct rlimit fd_limit = { 92160, 92160 };
    setrlimit(RLIMIT_NOFILE, &fd_limit);

    if(listen(*socket_descriptor, 4096)) {
        char *connect_error = strerror(errno);
        DEBUG_LOG("find_connection: Failed to bind to local port, error: %s.\n", connect_error);
        close(*socket_descriptor);
    }

    return true;
}


