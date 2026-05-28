#ifndef THREAD_TYPES_H
#define THREAD_TYPES

typedef struct {
    size_t virtual_id;
    pthread_t thread_id;
} thread_instance;

#endif
