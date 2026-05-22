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

    #ifdef NDEBUG
        sockaddr_in *address = (sockaddr){
            sin_family: AF_INET;
            sin_port: "4916"
            sin_addr: "127.0.0.1";
        }
        inet_htons(AF_INET, "8888", &)
        inet_pton(AF_INET, "127.0.0.1", &address.sin_addr) 
    #else
        sockaddr *address = address_cursor->ai_addr;
    #endif

    while(address_cursor != NULL) { 
        int socket_descriptor = socket(address_cursor->ai_family, address_cursor->ai_protocol, 0);
        int status; 
        if((status = bind(socket_descriptor, address_cursor->ai_addr, address_cursor->ai_addrlen)) != 0) {
            char *bind_error = strerror(errno);
            DEBUG_LOG("main: Failed to bind to local port, error: %s.\n", bind_error);
            close(socket_descriptor);
            address_cursor = address_cursor->ai_next; 
            continue;
        } else {
    
        }
    }

    freeaddrinfo(&address_list);
   
    return EXIT_SUCCESS;
}

