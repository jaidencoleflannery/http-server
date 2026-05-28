#include <sys/event.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <netdb.h>

#include "types/address-types/address_types.h"
#include "services/logging/logging.h"
#include "services/thread-handler/thread_handler.h"
#include "services/host-resolver/host_resolver.h"
#include "services/connection-handler/connection_handler.h"
#include "configuration/configuration-handler/configuration_handler.h"
#include "utilities/error-handler/error_handler.h"

#include "./orchestrator.h"

static void *process_request(void * client_descriptor) {
    char *buffer = calloc(1, RECEIVE_BUFFER_SIZE);
        size_t num_bytes_read = 0;
        // TODO: need to loop on this until the message is finished.
        if(!receive_data(*(int *)client_descriptor, 0, RECEIVE_BUFFER_SIZE, buffer, &num_bytes_read))
            ERROR_LOG("start_processing: Failed to receive data.");

        LOG("[ RECEIVED ]", "%s", buffer);
    return NULL;
}

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
    if(!get_socket_descriptor(socket_descriptor) || *socket_descriptor < 0) {
        ERROR_LOG("start_processing: Fatal error, failed to fetch socket_descriptor. Descriptor returned: %d.", *socket_descriptor);
        return false;
    }

    if(!init_thread_handler()) {
        ERROR_LOG("start_processing: Failed to initialize thread handler.");
        return false;
    }

    struct kevent data_event;
    int event_queue = kqueue();
    if(!validate_syscall(
        event_queue,
        "start_processing",
        "Failed to initialize kqueue.")
    ) {
        return false;
    }  

    // listen for new connections.
    EV_SET(&data_event, *socket_descriptor, EVFILT_READ, EV_ADD, 0, 0, NULL);
    if(!validate_syscall(
        kevent(event_queue, &data_event, 1, NULL, 0, NULL),
        "start_processing",
        "Failed to create kevent for main connection socket.")
    ) {
        return false;
    }


    // loop on event queue. 
    while(1) {
        struct kevent event;
        int num_events = kevent(event_queue, NULL, 0, &event, 1, NULL);
        if(!validate_syscall(
            num_events,
            "start_processing",
            "Critical failure, failed to poll for event.")
        ) {
            return false;
        }

        if(num_events > 0) {
            DEBUG_LOG("start_processing: Connection event flag signalled.");

            // for debug logging.
            #ifndef NDEBUG 
            char client[400];
            if(!get_host_name(client, 400))
                ERROR_LOG("start_processing: Could not resolve hostname for new connection.");
            DEBUG_LOG("Processing request on file descriptor: %d, for port: %zu.", *socket_descriptor, config.port);
            DEBUG_LOG("Connection received from: %s", client);
            #endif

            // new connection.
            if((int)event.ident == *socket_descriptor) { 
                sockaddr_storage *client_address = &(sockaddr_storage){ 0 };
                int *client_descriptor = calloc(1, sizeof(int));
                *client_descriptor = -1;
                if(!accept_connection(*socket_descriptor, client_address, client_descriptor)) {
                    ERROR_LOG("start_processing: Failed to accept connection.");
                    continue;
                }

                if(*client_descriptor == -1) {
                    ERROR_LOG("start_processing: The stack variable client_descriptor was not properly set when the connection was accepted.");
                    continue; // skip the connection; this should never happen.
                }

                // add new connection to queue so we can watch for data.
                struct kevent client_event;
                EV_SET(&client_event, *client_descriptor, EVFILT_READ, EV_ADD | EV_ONESHOT, 0, 0, NULL);
                if(!validate_syscall(
                    kevent(event_queue, &client_event, 1, NULL, 0, NULL),
                    "start_processing",
                    "Failed to add event for new connection.")
                ) {
                    continue;
                }
            } else {
            // connection received data, invoke a thread to process.
            }
        }
    }
}

