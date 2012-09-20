#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdbool.h>

bool read_config(const char *cfgfile, char ***paths);

#endif
