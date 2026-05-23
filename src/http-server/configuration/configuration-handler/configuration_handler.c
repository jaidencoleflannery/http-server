#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "services/logging/logging.h"
#include "configuration/configuration-values/configuration_values.h"

#include "./configuration_handler.h"

static configuration config_cache = (configuration){
    .max_connections = DEFAULT_MAX_CONNECTIONS,
    .port = DEFAULT_PORT 
};

bool initialize_configuration() {
    FILE *configuration_file = fopen(CONFIG_FILE, READ_ONLY);

    const char **validated_names = configuration_names;
    const char **validated_names_cursor = validated_names;

    char line[MAX_FIELD_LENGTH + 1];
    while(fgets(line, MAX_FIELD_LENGTH, configuration_file)) {
        char *line_cursor = &line[0];

        char field_name_cache[MAX_SECTION_LENGTH]; 
        // get field name. 
        for(int char_index = 0; char_index < MAX_SECTION_LENGTH; char_index++) {
            if(*line_cursor == ' ')
                break;

            field_name_cache[char_index] = *line_cursor;
            ++line_cursor;
        }

        DEBUG_LOG("initialize_configuration: Read configuration field name: %s.\n", field_name_cache);

        // validate field name.
        bool name_validated = false;
        while(validated_names_cursor) {
            if(strcmp(field_name_cache, *validated_names_cursor)) {
                name_validated = true;
                break;
            }
        }

        if(!name_validated) {
            ERROR_LOG("Could not validate name of configuration field: %s.\n", field_name_cache);
            return false;
        }

        // manually push through assignment.
        ++line_cursor;
        if(*line_cursor != '=') {
            ERROR_LOG("Configuration file is malformed around field name: %s.\n", field_name_cache);
            return false;
        }
        ++line_cursor;
        if(*line_cursor != ' ') {
            ERROR_LOG("Configuration file is malformed around field name: %s.\n", field_name_cache);
            return false;
        }

        // get field value.
        char field_value_cache[MAX_SECTION_LENGTH]; 
        for(int char_index = 0; char_index < MAX_SECTION_LENGTH; char_index++) {
            if(*line_cursor == '\0')
                break;

            field_value_cache[char_index] = *line_cursor;
        }

        DEBUG_LOG("initialize_configuration: Read configuration field value: %s.\n", field_value_cache);


    }

    return false;
}

