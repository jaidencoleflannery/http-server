#ifndef THREAD_HANDLER_H
#define THREAD_HANDLER_H

bool poll_event(int file_descriptor, void *function);

bool init_thread_handler();

#endif
