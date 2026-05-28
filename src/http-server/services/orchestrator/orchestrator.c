#include <stdlib.h>
#include <stdbool.h>
#include <netdb.h>
#include <pthread.h>

#include "types/address-types/address_types.h"
#include "types/thread-types/thread_types.h"
#include "services/logging/logging.h"
#include "services/host-resolver/host_resolver.h"
#include "services/connection-handler/connection_handler.h"
#include "configuration/configuration-handler/configuration_handler.h"
#include "utilities/error-handler/error_handler.h"

#include "./orchestrator.h"

bool boot_server() {
    LOG("[ ORB ]", "Booting server.");

    if(!initialize_configuration()) {
        ERROR_LOG("boot_server: Failed to initialize configuration.");
        return false;
    }
 
    addrinfo *addresses = { 0 }; 
    if(!get_local_addresses(false, &addresses)) {
        ERROR_LOG("boot_server: Failed to fetch local addresses.");
        return false;
    } 

    addrinfo bound_address; 
    if(!find_listen(addresses, &bound_address)) {
        ERROR_LOG("boot_server: Failed to listen to local address.");
        return false;
    }

    freeaddrinfo(addresses);

    LOG("[ ORB ]", "Listening on port: %zu.", config.port);
    return true;
}

bool start_processing() {
    int *socket_descriptor = &(int){ -1 };
    if(!get_socket_descriptor(socket_descriptor)) {
        ERROR_LOG("start_processing: Fatal error, failed to fetch socket_descriptor. Descriptor returned: %d.", *socket_descriptor);
        return false;
    }

    thread_instance thread[config.num_cores]; // prealloc pool based on num of performance cores.

    while(1) {
        sockaddr_storage *client_address = &(sockaddr_storage){ 0 };
        int client_descriptor = -1;
        while(client_descriptor < 0)
            client_descriptor = accept(*socket_descriptor, (sockaddr *)client_address, &(socklen_t){ sizeof(sockaddr) }); 

        pthread_create(&thread, NULL, worker, &id);

        char client[400];
        get_host_name(client, 400);
        
        DEBUG_LOG("Processing request on file descriptor: %d, for port: %zu.", *socket_descriptor, config.port);
        LOG("[ CONNECTION ]", "Connection received from: %s", client);

        char *buffer = calloc(1, RECEIVE_BUFFER_SIZE);
        size_t num_bytes_read = 0;
        if(!receive_data(client_descriptor, 0, RECEIVE_BUFFER_SIZE, buffer, &num_bytes_read))
            ERROR_LOG("start_processing: Failed to receive data.");

        LOG("[ RECEIVED ]", "%s", buffer);
    }

    return true;
}

