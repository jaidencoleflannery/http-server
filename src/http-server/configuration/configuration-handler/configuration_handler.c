#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "services/logging/logging.h"

#include "./configuration_handler.h"

static configuration config_cache = (configuration){
    .max_connections = DEFAULT_MAX_CONNECTIONS,
    .port = DEFAULT_PORT 
};

bool initialize_configuration() {
    FILE *configuration_file = fopen(CONFIG_FILE, READ_ONLY);
    char line[MAX_FIELD_LENGTH];

    while(fgets(line, MAX_FIELD_LENGTH, configuration_file)) {
        char *cursor = &line[0];
        char field_cache[MAX_NAME_LENGTH]; 
        // validate configuration field name. 
        for(int char_index = 0; char_index < MAX_NAME_LENGTH; char_index++) {
            if(*cursor == ' ')
                break;

            field_cache[char_index] = *cursor;
        }

        if(strcmp())
        return false;
    }

    return false;
}

