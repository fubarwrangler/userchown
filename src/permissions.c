#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <assert.h>
#include <pwd.h>
#include <limits.h>

#include "util.h"
#include "config.h"


bool file_allowed(const char *path, char **allowed)
{
	char **p = allowed;

	do	{
		if(strncmp(path, *p, strlen(*p)) == 0)
			return true;
	} while(*++p);

	return false;
}


void if_valid_become(const char *username, gid_t required_group)
{
	struct passwd *pw = NULL;
	gid_t mygid = getgid();

	pw = getpwnam(username);
	if(pw == NULL || errno)
		log_exit_perror(1, "getpwuid");

	if(pw->pw_gid != required_group)
		log_exit(4, "%s's group id is %d, but needs to be %d (rhphenix)",
			username, pw->pw_gid, required_group);

	if(pw->pw_gid != mygid)	{
		if(setgid(pw->pw_gid) != 0)
			log_exit_perror(4, "becoming group %d", pw->pw_gid);
	}

	if(setuid(pw->pw_uid) != 0)
		log_exit_perror(4, "becoming user %s", username);
}
