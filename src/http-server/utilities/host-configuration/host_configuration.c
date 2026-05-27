#include <stdbool.h>
#include <stdlib.h>

#include "utilities/error-handler/error_handler.c"

#include "./host_configuration.h"

bool configure_host() {
    // mac call to increase the number of available file descriptors.
    const struct rlimit fd_limit = { 92160, 92160 };
    setrlimit(RLIMIT_NOFILE, &fd_limit);

    struct rlimit fd_limit_container;
    int rlimit_result = getrlimit(RLIMIT_NOFILE, &fd_limit_container);
    if(!validate_syscall(rlimit_result, "configure_host", "Error fetching rate limit. Cannot confirm host configuration ran properly."))
        return false;

    if(fd_limit_container.rlim_cur != 92160 || fd_limit_container.rlim_max != 92160) {
        ERROR_LOG(
            "configure_host: Rate limit could not be changed, host configuration failed, rate limit: { %lu, %lu}.\n", 
            (unsigned long)fd_limit_container.rlim_cur, (unsigned long)fd_limit_container.rlim_max
        );
        return false;
    }

    return true;
}

