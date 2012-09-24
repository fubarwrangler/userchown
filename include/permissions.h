#ifndef PERMISSIONS_H_
#define PERMISSIONS_H_

#include <stdbool.h>

void if_valid_become(const char *username, const char *required_group);
bool file_allowed(const char *path, char **allowed);
void die_unless_user(const char *user);


#endif
