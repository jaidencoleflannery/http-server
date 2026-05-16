#ifndef GENERIC_TYPES_H
#define GENERIC_TYPES_H

#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define INET_PORTSTRLEN 6

typedef struct addrinfo addrinfo;
typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr_in6 sockaddr_in6;

typedef struct {
    char *name;
    size_t name_length;
} hostname;

#endif
