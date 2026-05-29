#ifndef THREAD_HANDLER_H
#define THREAD_HANDLER_H

#define RECEIVE_BUFFER_SIZE 16384
#define NUM_CONNECTIONS 32768

bool init_thread_handler();

bool invoke_thread(int client_descriptor);

#endif
