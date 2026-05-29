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

static thread_instance *threads; // holds all threads contiguously.
static pthread_mutex_t lock;
static pthread_cond_t lock_available;
static connection_instance *queue_head; // linked list of connections.
static connection_instance *queue_tail;
static int count = 0;

/*
 * thread_handler implements the producer consumer pattern.
 * the producer is the queue of connections, which the threads pull from.
 * whichever threads holds the mutex is waiting for a connection to arrive in the queue, once a connection arrives, it drops the mutex and starts processing that connection.
 * all other threads wait for the mutex to become available, and repeat the aforementioned behavior. 
 */

bool queue_task(int client_descriptor) {
    queue_tail->next = calloc(1, sizeof(connection_instance));
    if(queue_tail->next == NULL) {
        ERROR_LOG("queue_task: Failed to allocate memory for task.");
        return false;
    }

    queue_tail->next->previous = queue_tail;
    queue_tail = queue_tail->next;

    ++count;
    return true;
}

// returns the file descriptor for the connection.
bool dequeue_task(int *result) {
    connection_instance *task = queue_head->next;
    if(task == NULL) {
        ERROR_LOG("dequeue_task: Failure, no task found.");
        return false;
    }

    *result = task->socket_descriptor;
    if(result == NULL || *result < 0) {
        ERROR_LOG("dequeue_task: Failure, socket descriptor in queue was invalid.");
        *result = -1;
        return false;
    }

    queue_head->next = queue_head->next->next;
    queue_head->next->previous = queue_head;

    free(task);
    --count;
    return true;
}

static bool process_request(int socket_descriptor) {
    char *buffer = calloc(1, RECEIVE_BUFFER_SIZE);
    if(buffer == NULL) {
        ERROR_LOG("process_request: Failed to allocate memory for buffer.");
        return false;
    }

    // TODO: need to loop on this until the message is finished.
    size_t num_bytes_read = 0;
    while(1) {
        if(!receive_data(socket_descriptor, 0, (RECEIVE_BUFFER_SIZE - 1), buffer, &num_bytes_read)) {
            ERROR_LOG("start_processing: Failed to receive data.");
            return false;
        }

        buffer[RECEIVE_BUFFER_SIZE] = '\n';

        LOG("[ MESSAGE ]", "%s...", buffer);

        if(num_bytes_read < 1) {
            DEBUG_LOG("process_request: Hit end of message.");
            return true;
        }
    }

    LOG("[ RECEIVED ]", "%s", buffer);
    return true;
}

static void *thread_runner(void * client_descriptor) {
    while(1) {
        pthread_mutex_lock(&lock);
        while(count == 0)
            pthread_cond_wait(&lock_available, &lock);

        int *socket_descriptor = &(int){ -1 };
        if(!dequeue_task(socket_descriptor)) {
            ERROR_LOG("process_request: Failed to fetch socket_descriptor from queue.");
            return false;
        }

        pthread_mutex_unlock(&lock);
        
        if(!process_request(*socket_descriptor)) {
            ERROR_LOG("thread_runner: Unable to process request.");
            return NULL;
        }
    }

    return NULL;
}

bool init_thread_handler(void) {
    // initialize mutex values.
    if(pthread_mutex_init(&lock, NULL) != 0) {
        ERROR_LOG("init_thread_handler: Fatal error, failed to initialize mutex.");
        return false;
    }

    if(pthread_cond_init(&lock_available, NULL) != 0) {
        ERROR_LOG("init_thread_handler: Fatal error, failed to initialize mutex condition.");
        return false;
    }

    // prealloc pool based on num of performance cores.
    threads = calloc(1, (config.num_cores * sizeof(thread_instance)));
    if(threads == NULL) {
        ERROR_LOG("init_thread_handler: Fatal error, failed to allocate memory for thread pool.");
        return false;
    }

    // start threads and track values.
    for(int cursor = 0; cursor < config.num_cores; cursor++) {
        *(threads + cursor) = (thread_instance){
            .virtual_id = cursor,
            .thread_id = (pthread_t)-1
        };

        if(!validate_syscall(
            pthread_create(&(threads + cursor)->thread_id, NULL, thread_runner, NULL),
            "start_processing",
            "Could not create a thread for processing.")
        ) { return false; }

        if((threads + cursor)->thread_id == (pthread_t)-1) {
            ERROR_LOG("init_thread_handler: Fatal error, failed to create thread.");
            return false;
        }
    }
 
    // initialize queue of connections.
    queue_head = calloc(1, sizeof(connection_instance));
    queue_tail = queue_head;

    return true;
}

