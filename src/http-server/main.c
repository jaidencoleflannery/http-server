#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <stddef.h>

#include "types/address-types/address_types.h"

#include "services/logging/logging.h"
#include "services/host-resolver/host_resolver.h"
#include "services/connection-handler/connection_handler.h"
#include "configuration/configuration-handler/configuration_handler.h"

int main(void) {
    if(!initialize_configuration()) {
        ERROR_LOG("main: Failed to initialize configuration.\n");
        return EXIT_FAILURE;
    }

    size_t max_connections = 0;
    if(!fetch_configuration_by_name("max_connections", &max_connections)) {
        ERROR_LOG("main: Failed to fetch max_connections by name.\n");
        return EXIT_FAILURE;
    }

    LOG("[ max_connections by name ]", "%zu.\n", max_connections);

    config_values max_connections_enum = CONFIG_MAX_CONNECTIONS;
    max_connections = 0;
    if(!fetch_configuration_by_enum(max_connections_enum, &max_connections)) {
        ERROR_LOG("main: Failed to fetch max_connections by enum value.\n");
        return EXIT_FAILURE;
    }

    LOG("[ max_connections by enum ]", "%zu.\n", max_connections);

    size_t port = 0;
    if(!fetch_configuration_by_name("port", &port)) {
        ERROR_LOG("main: Failed to fetch port by name.\n");
        return EXIT_FAILURE;
    }

    LOG("[ port by name ]", "%zu.\n", port);

    config_values port_value_enum = CONFIG_PORT;
    port = 0;
    if(!fetch_configuration_by_enum(port_value_enum, &port)) {
        ERROR_LOG("main: Failed to fetch port by enum value.\n");
        return EXIT_FAILURE;
    }

    LOG("[ port by enum ]", "%zu.\n", port);

    addrinfo addresses; 
    if(!get_local_addresses(true, &addresses)) {
        ERROR_LOG("main: Failed to fetch local addresses.\n");
        return EXIT_FAILURE;
    }

    if(!find_connection(&addresses)) {
        ERROR_LOG("main: Failed to connect to local address.\n");
        return EXIT_FAILURE;
    }
    
    freeaddrinfo(&addresses); 
    return EXIT_SUCCESS;
}

