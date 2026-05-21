#ifndef HOST_RESOLVER_H
#define HOST_RESOLVER_H

#include "../../types/address_types.h"

bool get_addresses(char *target_name, bool is_https, ipv4_node **ipv4_list_head, ipv6_node **ipv6_list_head);

bool print_addresses(char *target_name, bool is_https);

#endif
