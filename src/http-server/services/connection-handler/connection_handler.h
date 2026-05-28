#ifndef CONNECTION_HANDLER_H
#define CONNECTION_HANDLER_H

bool find_connection(addrinfo *addresses);

bool find_listen(addrinfo *addresses, addrinfo *bound_address);

bool receive_data(int file_descriptor, int flags, size_t buffer_length, char *buffer, size_t *num_bytes_read);

bool get_socket_descriptor(int *result);

bool get_host_name(char *result, size_t result_size);

#endif
