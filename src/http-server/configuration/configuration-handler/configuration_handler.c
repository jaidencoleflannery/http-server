#include <sys/stat.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stddef.h>

#include "services/logging/logging.h"

#include "./configuration_handler.h"

// configuration values, defaults on init.
static configuration config_cache = (configuration){
    .max_connections = DEFAULT_MAX_CONNECTIONS,
    .port = DEFAULT_PORT 
};

static bool parse_configuration(void) { 
    // validate folder path.
    struct stat status;
    if(stat(CONFIG_FOLDER, &status) != 0) {
        ERROR_LOG("Configuration folder could not be found. This program expects a `./configuration/` folder in root. Error: %s.\n", strerror(errno));
        return false;
    }
 
    FILE *configuration_file = fopen(CONFIG_FILE, READ_ONLY);
    if(configuration_file == NULL) {
        ERROR_LOG("Could not open configuration file from folder: `./configuration/`. Error: %s.\n", strerror(errno));
        return false;
    } 

    // read.
    char line[MAX_FIELD_LENGTH];
    while(fgets(line, MAX_FIELD_LENGTH, configuration_file)) {
        char *line_cursor = line;

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
        const cfg_entry *valid_entries = cfg_entries;
        const cfg_entry *entry_field;
        bool name_validated = false; 

        while(valid_entries != NULL) {
            if(strcmp(field_name_cache, valid_entries->name) == 0) {
                name_validated = true;
                entry_field = valid_entries;
                break;
            }
            ++valid_entries;
        }

        if(!name_validated) {
            ERROR_LOG("Could not validate name of configuration field: %s.\n", field_name_cache);
            return false;
        }

        // manually push through assignment section.
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
        ++line_cursor;
        if(*line_cursor < '0' || *line_cursor > '9') {
            ERROR_LOG("Configuration file is malformed around field name: %s.\n", field_name_cache);
            return false;
        }

        // get field value.
        char field_value_cache[MAX_SECTION_LENGTH + 1]; // fgets appends a null terminator.
        for(int char_index = 0; char_index < (MAX_SECTION_LENGTH + 1); char_index++) {
            if(!*line_cursor)
                break;

            field_value_cache[char_index] = *line_cursor;
            ++line_cursor;
        }

        DEBUG_LOG("initialize_configuration: Read configuration field value: %s.\n", field_value_cache);

        // convert string value into a number.
        char *value_cursor = field_value_cache;
        size_t value = 0;
        while(value_cursor && *value_cursor != '\n') {
            int current_value = (int)*value_cursor;
            if(current_value < 48 || current_value > 57) {
                ERROR_LOG("Configuration field value is invalid, values must all be positive non-zero integer types.");
                return false;
            }

            value += (current_value - '0');
            ++value_cursor;
        }

        if(value <= 0) {
            ERROR_LOG("Configuration field value is invalid, values must all be positive non-zero integer types.");
            return false;
        }

        memcpy((unsigned char *)&config_cache + entry_field->offset, &value, sizeof(value));
        LOG("[ Configuration ]", "Set value to: %s = %zu.\n", field_name_cache, value);
    } 
    return true;
}

bool initialize_configuratin(void) {
    if(!parse_configuration()) {
        ERROR_LOG("Defaulting to default configuration.\n");
        return false;
    } else {
        DEBUG_LOG("Configuration loaded successfully.\n");
        return true;
    }
}

