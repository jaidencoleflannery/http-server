#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <stddef.h>

#include "services/logging/logging.h"
#include "services/orchestrator/orchestrator.c"

int main(void) {
    if(!boot_server()) {
        ERROR_LOG("main: Failed to boot.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

