#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#include "types/thread-types/thread_types.h"
#include "services/logging/logging.h"
#include "configuration/configuration-handler/configuration_handler.h"

#include "./thread_handler.h"

// TODO: virtualize kevent fd -> threads somehow.
static thread_instance *threads; // prealloc pool based on num of performance cores.

bool init_thread_handler() {
    threads = calloc(1, config.num_cores);
    if(threads == NULL) {
        ERROR_LOG("init_thread_handler: Failed to allocate memory.");
        return false;
    }

    return true;
}

bool poll_event(int file_descriptor, void *function) {
    while(1) {
        int n = 

    }
    return true;
}

bool free_pool() {
    free(threads);
    return true;
}


example:

// create the thread.
        pthread_t thread_id = 0;
        validate_syscall(
            pthread_create(&thread_id, NULL, process_request, &client_descriptor),
            "start_processing",
            "Could not create a thread for processing."
        ); 

        validate_syscall(
            pthread_create(&thread_id, NULL, process_request, &client_descriptor),
            "start_processing",
            "Could not create a thread for processing."
        );
