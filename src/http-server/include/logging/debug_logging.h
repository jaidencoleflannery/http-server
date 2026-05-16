#ifndef DEBUG_LOGGING_H
#define DEBUG_LOGGING_H

#include <stdio.h>

#ifndef NDEBUG
#define DEBUG_LOG(input, ...) fprintf(stderr, input, ##__VA_ARGS__)
#else
#define DEBUG_LOG(input) ((void)0)
#endif

#endif

