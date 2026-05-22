#include <stdbool.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>

#include "types/address-types/address_types.h"
#include "services/logging/logging.h"

#include "./host_resolver.h"

bool get_local_addresses(bool is_https, addrinfo *address_list) { 
    DEBUG_LOG("get_local_addresses: Searching for local address.\n");

    addrinfo address_request;
    // make sure our request memory is clean.
    memset(&address_request, 0, sizeof address_request);
    address_request = (addrinfo){
        .ai_family = AF_UNSPEC, // any ip version.
        .ai_socktype = SOCK_STREAM, // tcp.
        .ai_flags = AI_PASSIVE // current machines ip.
    };

    int status;
    // TODO: switch the port to be configurable.
    if((status = getaddrinfo(NULL, PORTSTRING, &address_request, &address_list)) != 0) { 
        ERROR_LOG("get_local_addresses: Error resolving addresses: %s\n", gai_strerror(status));
        return false;
    }

    DEBUG_LOG("get_local_addresses: Retrieved addresses from current machine.\n");
    return true;
}

bool print_local_addresses(ipv4_node *ipv4_list, ipv6_node *ipv6_list) {
    DEBUG_LOG("print_addresses: Printing provided addresses.\n");

    ipv4_node *ipv4_cursor = ipv4_list->next;
    ipv4_node *ipv4_cleanup_cursor = ipv4_cursor;
    while(ipv4_cursor != NULL) {
        char ipv4_address[INET_ADDRSTRLEN];
        void *binary_address = &(ipv4_cursor->sockaddr_in.sin_addr);
        if(inet_ntop(AF_INET, binary_address, ipv4_address, sizeof(ipv4_address)) == NULL) {
            DEBUG_LOG("print_addresses: Failed to convert an IPv4 binary address into a string.\n");
            continue;
        }

        uint16_t ipv4_port = ntohs(ipv4_cursor->sockaddr_in.sin_port);

        LOG("[ IPv4 ]", "%s:%u.\n", ipv4_address, ipv4_port);
        ipv4_cursor = ipv4_cursor->next;

        // free memory on the tail.
        free(ipv4_cleanup_cursor);
        ipv4_cleanup_cursor = ipv4_cursor;
    }

    // free lingering tail.
    free(ipv4_cleanup_cursor);

    ipv6_node *ipv6_cursor = ipv6_list->next;
    ipv6_node *ipv6_cleanup_cursor = ipv6_cursor;
    while(ipv6_cursor != NULL) {
        char ipv6_address[INET6_ADDRSTRLEN];
        void *binary_address = &(ipv6_cursor->sockaddr_in6.sin6_addr);
        if(inet_ntop(AF_INET6, binary_address, ipv6_address, sizeof(ipv6_address)) == NULL) {
            DEBUG_LOG("print_addresses: Failed to convert an IPv6 binary address into a string.\n");
            continue;
        }

        uint16_t ipv6_port = ntohs(ipv6_cursor->sockaddr_in6.sin6_port);

        LOG("[ IPv6 ]", "%s:%u.\n", ipv6_address, ipv6_port);
        ipv6_cursor = ipv6_cursor->next;
        
        // free memory on the tail.
        free(ipv6_cleanup_cursor);
        ipv6_cleanup_cursor = ipv6_cursor;
    }

    // free lingering tail.
    free(ipv4_cleanup_cursor);

    LOG("[ LOG ]", "List exhausted.\n");
    return true;
}

