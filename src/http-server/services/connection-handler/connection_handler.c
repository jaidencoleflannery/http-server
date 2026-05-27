#include <sys/socket.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "types/address-types/address_types.h"
#include "services/logging/logging.h"
#include "configuration/configuration-handler/configuration_handler.h"
#include "utilities/error-handler/error_handler.h"

#include "./connection_handler.h"

bool find_bind(addrinfo *addresses, int **socket_descriptor, sockaddr_in **bound_address) {
    const size_t port = config.port;

    sockaddr_in *address;

    // if debugging use localhost loopback.
    #ifndef NDEBUG
        address = &(sockaddr_in){
            .sin_family = AF_INET,
            .sin_port = htons(port),
        };
        inet_pton(AF_INET, IPV4_LOCALHOST, &address->sin_addr);
    #endif

    while(addresses->ai_addr) {
        if(!address)
            address = (sockaddr_in *)addresses->ai_addr;

        int testing_descriptor;
        if((testing_descriptor = socket(address->sin_family, SOCK_STREAM, 0)) < 0) {
            char *socket_error = strerror(errno);
            DEBUG_LOG("find_bind: Failed to open a socket, error: %s.\n", socket_error);
            addresses = addresses->ai_next;
            continue;
        }

        setsockopt(testing_descriptor, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));

        if(bind(testing_descriptor, (sockaddr *)address, sizeof(*address)) < 0) {
            char *bind_error = strerror(errno);
            DEBUG_LOG("find_bind: Failed to bind to local port, error: %s.\n", bind_error);
            close(testing_descriptor);
            addresses = addresses->ai_next; 
            continue;
        } else {
            if(!testing_descriptor) {
                ERROR_LOG("find_bind: Failed to allocate memory for port bind.\n");
                return false;
            }
            *socket_descriptor = memcpy(*socket_descriptor, &testing_descriptor, sizeof(testing_descriptor));
            *bound_address = memcpy(*bound_address, address, sizeof(*address));
            break;
        }
    }

    if(!socket_descriptor) {
        DEBUG_LOG("find_bind: No successful bindings for provided addresses.\n");
        return false;
    }

    return true;
}

// for communicating with external servers.
bool find_connection(addrinfo *addresses) {
    int *socket_descriptor = calloc(1, sizeof(int));
    sockaddr_in *bound_address = calloc(1, sizeof(sockaddr_in));

    // iterate, first fit algo.  
    if(!find_bind(addresses, &socket_descriptor, &bound_address)) {
        DEBUG_LOG("find_connection: Failed to find to any provided addresses.\n");
        return false;
    }

    if(socket_descriptor == NULL || bound_address == NULL) {
        ERROR_LOG("find_connection: socket_descriptor or bound_address were null.");
        return false;
    }

    if(connect(*socket_descriptor, (sockaddr *)bound_address, sizeof(sockaddr_in)) < 0) {
        char *connect_error = strerror(errno);
        DEBUG_LOG("find_connection: Failed to bind to local port, error: %s.\n", connect_error);
        close(*socket_descriptor);
        return false;
    }

    return true;
}

bool find_listen(addrinfo *addresses) {
    int *socket_descriptor = calloc(1, sizeof(int));
    sockaddr_in *bound_address = calloc(1, sizeof(sockaddr_in));

    // iterate, first fit algo.  
    if(!find_bind(addresses, &socket_descriptor, &bound_address)) {
        DEBUG_LOG("find_connection: Failed to find to any provided addresses.\n");
        return false;
    }

    // mac call to increase the number of available file descriptors.
    struct rlimit fd_limit = { 92160, 92160 };
    setrlimit(RLIMIT_NOFILE, &fd_limit);

    if(listen(*socket_descriptor, config.max_connections)) {
        char *connect_error = strerror(errno);
        DEBUG_LOG("find_connection: Failed to bind to local port, error: %s.\n", connect_error);
        close(*socket_descriptor);
        return false;
    }

    return true;
}

bool shutdown_connection(int file_descriptor, int type) {
    int shutdown_return = shutdown(file_descriptor, type);
    if(!validate_syscall(shutdown_return, "shutdown_connection", "Error encountered while attempting to close socket."))
        return false;

    return true;
}

bool accept_connection(int file_descriptor, sockaddr *address) {
    int accept_return = accept(file_descriptor, address, &(socklen_t){ sizeof(sockaddr_storage) });
    if(!validate_syscall(accept_return, "accept_connection", "Listening file descriptor did not give any data."))
        return false;

    return true;
}

bool send_data(int file_descriptor, char *data, size_t data_length, int flags) {
    size_t total_bytes_sent = 0;
    while(total_bytes_sent < data_length) {
        size_t bytes_sent = send(file_descriptor, (data + bytes_sent), (data_length - bytes_sent), 0);
        if(!validate_syscall(bytes_sent, "send_data", "Failed to send data."))
            return false;
        if(bytes_sent == 0) {
            ERROR_LOG("send_data: Unexpected error occurred while attempting to send data.\n");
            return false;
        }

        total_bytes_sent += bytes_sent;
        bytes_sent = 0; 
    }

    return true;
}

bool receive_data(int file_descriptor, char *buffer, size_t buffer_length, int flags) {
    size_t bytes_received = recv(file_descriptor, buffer, buffer_length, 0);
    if(!validate_syscall(bytes_received, "send_data", "Failed to receive data.")) {
        return false;
    } else if(bytes_received == 0) {
        DEBUG_LOG("send_data: Remote connection was closed.\n");
        return false;
    }

    return true;
}

bool get_peer_name(int file_descriptor, sockaddr *result) {
    int peer_return = getpeername(file_descriptor, result, &(socklen_t){ sizeof(sockaddr) });
    if(!validate_syscall(peer_return, "peer_return", "Failed to receive data."))
        return false;

    return true;
}

