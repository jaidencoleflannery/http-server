#include <stdbool.h>
#include <stdio.h>

#include "./configuration_handler.h"

bool initialize_configuration() {
    fopen(CONFIG_FILE, READ_ONLY);

    return false;
}

