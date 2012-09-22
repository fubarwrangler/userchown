#ifndef PERMISSIONS_H_
#define PERMISSIONS_H_

#include <stdbool.h>

void if_valid_become(const char *username, gid_t required_group);
bool path_ok(const char *path);



#endif
