#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <netdb.h>

#include "types/thread-types/thread_types.h"
#include "services/logging/logging.h"
#include "configuration/configuration-handler/configuration_handler.h"
#include "utilities/error-handler/error_handler.h"
#include "services/connection-handler/connection_handler.h"

#include "./thread_handler.h"

// TODO: virtualize kevent fd -> threads somehow.
static thread_instance *threads;
static size_t task_counter = 0;

static void *process_request(void * client_descriptor) {
    char *buffer = calloc(1, RECEIVE_BUFFER_SIZE);
        size_t num_bytes_read = 0;
        // TODO: need to loop on this until the message is finished.
        if(!receive_data(*(int *)client_descriptor, 0, RECEIVE_BUFFER_SIZE, buffer, &num_bytes_read))
            ERROR_LOG("start_processing: Failed to receive data.");

        LOG("[ RECEIVED ]", "%s", buffer);
    return NULL;
}

bool init_thread_handler() {
    // prealloc pool based on num of performance cores.
    threads = calloc(1, config.num_cores * sizeof(thread_instance));
    if(threads == NULL) {
        ERROR_LOG("init_thread_handler: Fatal error, failed to allocate memory for threads.");
        return false;
    }

    for(int cursor = 0; cursor < config.num_cores; cursor++) {
        thread_instance *thread = calloc(1, sizeof(thread_instance));
        *thread = (thread_instance){
            .virtual_id = cursor,
            .thread_id = calloc(1, sizeof(pthread_t))
        };

        if(!validate_syscall(
            pthread_create(&(thread->thread_id), NULL, process_request, NULL),
            "start_processing",
            "Could not create a thread for processing.")
        ) {
            return false;
        }
    }
    return true;
}

bool free_pool() {
    free(threads);
    return true;
}

bool invoke_thread(int client_descriptor) {
    if(!validate_syscall(
        // round robin. <- actually, track which threads are actually available and round robin on those.
        pthread_join(threads[task_counter % config.num_cores], NULL, process_request, &client_descriptor),
        "start_processing",
        "Could not create a thread for processing.")
    ) {
        return false;
    }
}


example:

// create the thread.
        
