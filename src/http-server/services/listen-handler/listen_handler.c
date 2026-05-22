#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "types/address-types/address_types.h"
#include "services/logging/logging.h"

#include "./listen_handler.h"

bool find_listen(addrinfo *addresses) {
    // successful connect fd.
    int *socket_descriptor = NULL;

    // iterate, first fit algo. 
    while(addresses != NULL) { 
        // if debugging use localhost loopback.
        #ifndef NDEBUG
            sockaddr_in address = (sockaddr_in){
                .sin_family = AF_INET,
                .sin_port = htons(PORT)
            };
            inet_pton(AF_INET, IPV4_LOCALHOST, &address.sin_addr);
        #else
            sockaddr *address = addresses->ai_addr;
        #endif 

        int testing_descriptor;
        if((testing_descriptor = socket(addresses->ai_family, addresses->ai_protocol, 0)) < 0) {
            char *socket_error = strerror(errno);
            DEBUG_LOG("connection_handler: Failed to open a socket, error: %s.\n", socket_error);
            addresses = addresses->ai_next; 
            continue;
        }

        setsockopt(testing_descriptor, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

        if(bind(testing_descriptor, addresses->ai_addr, addresses->ai_addrlen) < 0) {
            char *bind_error = strerror(errno);
            DEBUG_LOG("connection_handler: Failed to bind to local port, error: %s.\n", bind_error);
            close(testing_descriptor);
            addresses = addresses->ai_next; 
            continue;
        } else {
            socket_descriptor = memcpy(malloc(sizeof(testing_descriptor)), &testing_descriptor, sizeof(testing_descriptor));
            break;
        }
    }

    if(!socket_descriptor) {
        DEBUG_LOG("connection_handler: No successful connections for provided addresses.");
        return false;
    }

    if(connect(*socket_descriptor, addresses->ai_addr, (unsigned int)addresses->ai_addrlen) < 0) {
        char *connect_error = strerror(errno);
        DEBUG_LOG("connection_handler: Failed to bind to local port, error: %s.\n", connect_error);
        close(*socket_descriptor);
    }

    return true;
}

