#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>

#include "util.h"
#include "config.h"


bool file_allowed(const char *path, char **allowed)
{
	do	{
		if(strncmp(path, *allowed, strlen(*allowed)) == 0)
			return true;
	} while(*++allowed);

	return false;
}


void if_valid_become(const char *username, const char *required_group)
{
	struct passwd *pw = NULL;
	struct group *gr = NULL;
	gid_t mygid = getgid();

	errno = 0;
	pw = getpwnam(username);
	if(pw == NULL)
		log_exit(1, "user %s not found", username);
	else if(errno != 0)
		log_exit_perror(1, "getpwnam");

	errno = 0;
	gr = getgrnam(required_group);
	if(gr == NULL)
		log_exit(1, "required-group %s not found", required_group);
	else if(errno != 0)
		log_exit_perror(1, "getgrname");

	if(pw->pw_gid != gr->gr_gid)
		log_exit(4, "%s's group id is %d, but needs to be %d (%s)",
			username, pw->pw_gid, gr->gr_gid, required_group);

	if(pw->pw_gid != mygid)	{
		if(setgid(pw->pw_gid) != 0)
			log_exit_perror(4, "becoming group %d", pw->pw_gid);
	}

	if(setuid(pw->pw_uid) != 0)
		log_exit_perror(4, "becoming user %s", username);
}
