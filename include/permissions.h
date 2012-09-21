#ifndef PERMISSIONS_H_
#define PERMISSIONS_H_

#include <stdbool.h>

bool path_ok(const char *path);
void if_valid_become(const char *username);


#endif
