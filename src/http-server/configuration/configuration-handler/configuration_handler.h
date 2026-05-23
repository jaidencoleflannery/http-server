#ifndef CONFIGURATION_HANDLER_H
#define CONFIGURATION_HANDLER_H

#ifndef NDEBUG
#define CONFIG_FILE "./configuration/release.cfg"
#else
#define CONFIG_FILE "./configuration/development.cfg"
#endif

#define DEFAULT_MAX_CONNECTIONS 4096
#define DEFAULT_PORT 8888
#define MAX_FIELD_LENGTH 200
#define MAX_SECTION_LENGTH (MAX_FIELD_LENGTH / 2)
#define READ_ONLY "r"

// keep fields the same data type so validation is straightforward.
typedef struct {
    size_t max_connections;
    size_t port;
} configuration;

bool initialize_configuration();

#endif
