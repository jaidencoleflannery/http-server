#include <stdlib.h>
#include <stdbool.h>
#include <netdb.h>

#include "types/address-types/address_types.h"
#include "services/logging/logging.h"
#include "services/host-resolver/host_resolver.h"
#include "services/connection-handler/connection_handler.h"
#include "configuration/configuration-handler/configuration_handler.h"

#include "./orchestrator.h"

bool boot_server() {
    LOG("[ ORB ]", "Booting server.");

    if(!initialize_configuration()) {
        ERROR_LOG("boot_server: Failed to initialize configuration.");
        return EXIT_FAILURE;
    }

    addrinfo *addresses;
    if(!get_local_addresses(false, &addresses)) {
        ERROR_LOG("boot_server: Failed to fetch local addresses.");
        return EXIT_FAILURE;
    } 

    addrinfo bound_address;
    if(!find_listen(addresses, &bound_address)) {
        ERROR_LOG("boot_server: Failed to listen to local address.");
        return EXIT_FAILURE;
    }

    freeaddrinfo(addresses);

    LOG("[ ORB ]", "Listening on port: %zu.", config.port);
    LOG("[ ORB ]", "Full address: %zu.", config.port);
    LOG("[ ORB ]", "Exit (q):");

    while(getchar() != 'q');
    
    return EXIT_SUCCESS;
}

