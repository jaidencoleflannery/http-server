#include <stdbool.h>
#include <stdlib.h>

#include "services/logging/logging.h"

#include "./string_tools.h"

bool string_length(char *target, size_t *result) {
    if(result == NULL) {
        DEBUG_LOG("string_length: Result pointer was null.\n");
        return false;
    }

    if(target == NULL)
        result = 0;
    else
        while(*target != '\0') {
            ++result;
            ++target;
        }

    return true;
}

