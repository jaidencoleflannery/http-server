#include <stdlib.h>
#include <stdbool.h>

#include "types/address-types/address_types.h"

#include "services/logging/logging.h"
#include "services/host-resolver/host_resolver.h"

int main(int argc, char **argv) {
    ipv4_node *ipv4_list_head;
    ipv6_node *ipv6_list_head;
    if(!get_addresses(argv[1], true, &ipv4_list_head, &ipv6_list_head)) {
        DEBUG_LOG("main: Failed to fetch addresses for: %s.\n", argv[1]);
        return -1;
    }

    if(!print_addresses(argv[1], true)) {
        DEBUG_LOG("main: Failed to fetch and print addresses for: %s.\n", argv[1]);
        return -1;
    }
   
    return EXIT_SUCCESS;
}

