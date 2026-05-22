#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>

#include "types/address-types/address_types.h"

#include "services/logging/logging.h"
#include "services/host-resolver/host_resolver.h"
#include "services/connection-handler/connection_handler.h"

int main(void) {
    addrinfo addresses; 
    if(!get_local_addresses(true, &addresses)) {
        DEBUG_LOG("main: Failed to fetch local addresses.\n");
        return EXIT_FAILURE;
    }

    if(!find_connection(&addresses)) {
        DEBUG_LOG("main: Failed to connect to local address.\n");
        return EXIT_FAILURE;
    }
    
    freeaddrinfo(&addresses); 
    return EXIT_SUCCESS;
}

