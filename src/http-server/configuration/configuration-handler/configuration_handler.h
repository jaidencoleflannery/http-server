#ifndef CONFIGURATION_HANDLER_H
#define CONFIGURATION_HANDLER_H

#ifndef NDEBUG
#define CONFIG_FILE "release.cfg"
#else
#define CONFIG_FILE "development.cfg"
#endif

#define READ_ONLY "r"

bool initialize_configuration();

#endif
