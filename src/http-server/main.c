#include <stdlib.h>

#include "./include/logging/debug_logging.h"
#include "./include/types/generic_types.h"

int main(int argc, char **argv) {
    static hostname target_host; 
    if(argc < 2 || *argv == NULL) {
        DEBUG_LOG("[ DEBUG ] main: No hostname was provided.\n");
        fprintf(stderr, "[ LOG ] Expected: ./resolvehost \"{target hostname}\".\n");
        return -1;
    } else {
        target_host = (hostname){ 
            .name = argv[1]
        };
        DEBUG_LOG("[ DEBUG ] main: Searching for addresses under %s.\n", target_host.name);
    }

    addrinfo address_request = (addrinfo){
        .ai_family = AF_UNSPEC,
        .ai_socktype = SOCK_STREAM,
    };

    addrinfo *addr_list;

    if(getaddrinfo("google.com", "80", &address_request, &addr_list) != 0) {
        DEBUG_LOG("[ DEBUG ] main: Failed to retrieve addresses for %s.\n", target_host.name);
        return -1;
    } else {
        DEBUG_LOG("[ DEBUG ] main: Retrieved addresses from %s.\n", target_host.name);
        addrinfo *addr_cursor = addr_list;

        ipv4_node ipv4_list_head = {0};
        ipv4_node *ipv4_cursor = &ipv4_list_head;

        ipv6_node ipv6_list_head = {0};
        ipv6_node *ipv6_cursor = &ipv6_list_head;

        DEBUG_LOG("[ DEBUG ] main: Casting address types.\n");
        while(addr_cursor != NULL) {
            // cast addresses to structs for their type.
            if(addr_cursor->ai_family == AF_INET) {
                sockaddr_in ipv4 = *(sockaddr_in *)addr_cursor->ai_addr;
                ipv4_cursor->next = malloc(sizeof(ipv4_node));
                *ipv4_cursor->next = (ipv4_node){ .sockaddr_in = ipv4 };
                ipv4_cursor = ipv4_cursor->next;
            } else if(addr_cursor->ai_family == AF_INET6) {
                sockaddr_in6 ipv6 = *(sockaddr_in6 *)addr_cursor->ai_addr;
                ipv6_cursor->next = malloc(sizeof(ipv6_node));
                *ipv6_cursor->next = (ipv6_node){ .sockaddr_in6 = ipv6 };
                ipv6_cursor = ipv6_cursor->next;
            }
            addr_cursor = addr_cursor->ai_next;
        }

        DEBUG_LOG("[ DEBUG ] main: Printing found IPv4 addresses.\n");
        ipv4_cursor = ipv4_list_head.next;
        ipv4_node *ipv4_cleanup_cursor = ipv4_cursor;
        while(ipv4_cursor != NULL) {
            char ipv4_address[INET_ADDRSTRLEN];
            void *binary_address = &(ipv4_cursor->sockaddr_in.sin_addr);
            inet_ntop(AF_INET, binary_address, ipv4_address, sizeof(ipv4_address));

            uint16_t ipv4_port = ntohs(ipv4_cursor->sockaddr_in.sin_port);

            printf("[ IPv4 ] %s:%u.\n", ipv4_address, ipv4_port);
            ipv4_cursor = ipv4_cursor->next;

            // free memory on the tail.
            free(ipv4_cleanup_cursor);
            ipv4_cleanup_cursor = ipv4_cursor;
        }

        DEBUG_LOG("[ DEBUG ] main: Printing found IPv6 addresses.\n");
        ipv6_cursor = ipv6_list_head.next;
        ipv6_node *ipv6_cleanup_cursor = ipv6_cursor;
        while(ipv6_cursor != NULL) {
            char ipv6_address[INET6_ADDRSTRLEN];
            void *binary_address = &(ipv6_cursor->sockaddr_in6.sin6_addr);
            inet_ntop(AF_INET6, binary_address, ipv6_address, sizeof(ipv6_address));

            uint16_t ipv6_port = ntohs(ipv6_cursor->sockaddr_in6.sin6_port);

            printf("[ IPv6 ] %s:%u.\n", ipv6_address, ipv6_port);
            ipv6_cursor = ipv6_cursor->next;
            
            // free memory on the tail.
            free(ipv6_cleanup_cursor);
            ipv6_cleanup_cursor = ipv6_cursor;
        }

        DEBUG_LOG("[ DEBUG ] main: Reached end of code.\n");
        printf("[ LOG ] List exhausted.\n");
    }
    
}

