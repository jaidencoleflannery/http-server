#ifndef THREAD_TYPES_H
#define THREAD_TYPES

typedef struct {
    int id;
    pthread_t pthread;
} thread_instance;

#endif
