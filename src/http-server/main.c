#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>

#include "types/address-types/address_types.h"

#include "services/logging/logging.h"
#include "services/host-resolver/host_resolver.h"

int main(int argc, char **argv) {
    addrinfo address_list; 
    if(!get_local_addresses(true, &address_list)) {
        DEBUG_LOG("main: Failed to fetch addresses for: %s.\n", argv[1]);
        return -1;
    }

    addrinfo *address_cursor = &address_list; 

    while(address_cursor != NULL) { 
        // if debugging use localhost loopback.
        #ifndef NDEBUG
            sockaddr_in address = (sockaddr_in){
                .sin_family = AF_INET,
                .sin_port = htons(PORT)
            };
            inet_pton(AF_INET, IPV4_LOCALHOST, &address.sin_addr);
        #else
            sockaddr *address = address_cursor->ai_addr;
        #endif

        int socket_descriptor;
        if((socket_descriptor = socket(address_cursor->ai_family, address_cursor->ai_protocol, 0)) < 0) {
            char *socket_error = strerror(errno);
            DEBUG_LOG("main: Failed to open a socket, error: %s.\n", socket_error);
            address_cursor = address_cursor->ai_next; 
            continue;
        }

        setsockopt(socket_descriptor, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

        int status; 
        if((status = bind(socket_descriptor, address_cursor->ai_addr, address_cursor->ai_addrlen)) != 0) {
            char *bind_error = strerror(errno);
            DEBUG_LOG("main: Failed to bind to local port, error: %s.\n", bind_error);
            close(socket_descriptor);
            address_cursor = address_cursor->ai_next; 
            continue;
        }
            
    }

    freeaddrinfo(&address_list);
   
    return EXIT_SUCCESS;
}

