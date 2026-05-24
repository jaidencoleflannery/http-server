#ifndef CONFIGURATION_HANDLER_H
#define CONFIGURATION_HANDLER_H

#ifndef NDEBUG
#define CONFIG_FILE "./configuration/release.cfg"
#else
#define CONFIG_FILE "./configuration/development.cfg"
#endif

#define CONFIG_FOLDER "configuration"
#define DEFAULT_MAX_CONNECTIONS 4096
#define DEFAULT_PORT 8888
#define MAX_FIELD_LENGTH 400
#define MAX_SECTION_LENGTH (MAX_FIELD_LENGTH / 2)
#define READ_ONLY "r"

typedef enum {
    CONFIG_MAX_CONNECTIONS,
    CONFIG_PORT,
    CONFIG_NULL,
} config_values;

// keep fields the same data type so validation is straightforward.
typedef struct {
    size_t  max_connections;
    size_t  port;
} configuration;

// table of entries for lookup.
typedef struct {
    char    *name;
    size_t  offset; // offset into struct configuration where the field exists.
    config_values type;
} cfg_entry;

static const cfg_entry cfg_entries[] = {
    { "max_connections", offsetof(configuration, max_connections), CONFIG_MAX_CONNECTIONS },
    { "port",            offsetof(configuration, port), CONFIG_PORT },
    { NULL, 0, CONFIG_NULL },
};

bool initialize_configuration(void);

bool fetch_configuration_by_name(char *target, size_t *value);

bool fetch_configuration_by_enum(config_values target, size_t *value);

#endif
