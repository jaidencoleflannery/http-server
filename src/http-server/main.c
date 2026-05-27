#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <stddef.h>

#include "services/logging/logging.h"
#include "services/orchestrator/orchestrator.c"
#include "utilities/host-configuration/host_configuration.c"

int main(void) {
    if(!configure_host()) {
        ERROR_LOG("main: Failed to configure host.\n");
        return EXIT_FAILURE;
    }

    if(!boot_server()) {
        ERROR_LOG("main: Failed to boot.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

