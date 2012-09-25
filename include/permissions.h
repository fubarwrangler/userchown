#ifndef PERMISSIONS_H_
#define PERMISSIONS_H_

void if_valid_become(const char *username, const char *required_group);
void validate_output(const char *path, char **allowed);
void die_unless_user(const char *user);


#endif
