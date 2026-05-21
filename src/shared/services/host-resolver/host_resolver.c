#include <stdbool.h>
#include <stdlib.h>
#include <netdb.h>

#include "host_resolver.h"

#include "../logging/logging.h"
#include "../../utilities/string-tools/string_tools.h"

bool get_addresses(char *target_name, bool is_https, ipv4_node **ipv4_list_head, ipv6_node **ipv6_list_head) {
    if(target_name == NULL) {
        DEBUG_LOG("get_addresses: No hostname was provided.\n");
        return false;
    }

    size_t name_length;
    if(!string_length(target_name, &name_length)) {
        DEBUG_LOG("get_addresses: Failed to fetch length of string.\n");
        return false;
    }

    hostname target_host = (hostname){ 
        .name = target_name,
        .name_length = name_length
    };

    DEBUG_LOG("get_addresses: Searching for addresses under %s.\n", target_host.name);

    addrinfo address_request = (addrinfo){
        .ai_family = AF_UNSPEC, // any ip version.
        .ai_socktype = SOCK_STREAM, // tcp.
    };

    addrinfo *addr_list;
    if(getaddrinfo(target_host.name, (is_https) ? "443" : "80", &address_request, &addr_list) != 0) {
        DEBUG_LOG("get_addresses: Failed to retrieve addresses for %s.\n", target_host.name);
        return false;
    }

    DEBUG_LOG("get_addresses: Retrieved addresses from %s.\n", target_host.name);
    addrinfo *addr_cursor = addr_list;

    *ipv4_list_head = malloc(sizeof(ipv4_node));
    ipv4_node *ipv4_cursor = *ipv4_list_head;

    *ipv6_list_head = malloc(sizeof(ipv6_node));
    ipv6_node *ipv6_cursor = *ipv6_list_head;

    DEBUG_LOG("get_addresses: Casting address types.\n");
    while(addr_cursor != NULL) {
        // cast addresses to structs for their type.
        if(addr_cursor->ai_family == AF_INET) {
            sockaddr_in *ipv4 = (sockaddr_in *)addr_cursor->ai_addr;
            ipv4_cursor->next = malloc(sizeof(ipv4_node));
            *ipv4_cursor->next = (ipv4_node){ .sockaddr_in = *ipv4 };
            ipv4_cursor = ipv4_cursor->next;
        } else if(addr_cursor->ai_family == AF_INET6) {
            sockaddr_in6 ipv6 = *(sockaddr_in6 *)addr_cursor->ai_addr;
            ipv6_cursor->next = malloc(sizeof(ipv6_node));
            *ipv6_cursor->next = (ipv6_node){ .sockaddr_in6 = ipv6 };
            ipv6_cursor = ipv6_cursor->next;
        }
        addr_cursor = addr_cursor->ai_next;
    } 
 
    return true;
}

bool print_addresses(char *target_name, bool is_https) {
    ipv4_node *ipv4_list_head;
    ipv6_node *ipv6_list_head;
    if(!get_addresses(target_name, is_https, &ipv4_list_head, &ipv6_list_head)) {
        DEBUG_LOG("print_addresses: No addresses were returned for provided hostname: %s\n", target_name);
        return false;
    }

    DEBUG_LOG("print_addresses: Printing found IPv4 addresses.\n");
    ipv4_node *ipv4_cursor = ipv4_list_head->next;
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

    DEBUG_LOG("print_addresses: Printing found IPv6 addresses.\n");
    ipv6_node *ipv6_cursor = ipv6_list_head->next;
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

