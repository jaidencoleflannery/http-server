#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "types/address-types/address_types.h"

#include "services/logging/logging.h"
#include "services/host-resolver/host_resolver.h"

int main(int argc, char **argv) {
    addrinfo *address_list;
    addrinfo *address_cursor = address_list;

    if(!get_local_addresses(true, &address_list)) {
        DEBUG_LOG("main: Failed to fetch addresses for: %s.\n", argv[1]);
        return -1;
    } 

    int *ipv4_sockets;
    while(ipv4_cursor != NULL) {
        sockaddr_in sock_address = ipv4_cursor->sockaddr_in;
        int sock = socket(sock_address.sin_family, sock_address.ai_protocol, 0);
    }

    int *ipv6_sockets;
    while(ipv6_cursor != NULL) {

        int sock = socket(ipv4_cursor->sockaddr_in.sin_family, SOCK_STREAM, 0);
    }
   
    return EXIT_SUCCESS;
}

