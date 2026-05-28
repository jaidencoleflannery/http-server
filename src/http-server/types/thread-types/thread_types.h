#ifndef THREAD_TYPES_H
#define THREAD_TYPES

typedef struct {
    int id;
    pthread_t thread;
} thread_instance;

#endif
