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


/* Don't return unless the path is allowed as per the allowed-list */
void validate_output(const char *path, char **allowed)
{
	bool path_ok = false;
	char *true_path;
	char *true_allowed;


	/* Expand all symbolic links and ../ references in the path */
	if((true_path = realpath(path, NULL)) == NULL)
		log_exit_perror(2, "Error expanding output path %s", path);

	do	{

		if((true_allowed = realpath(*allowed, NULL)) == NULL)
			log_exit_perror(2, "Error expanding config-file path %s",
				*allowed);

		if(strncmp(path, true_allowed, strlen(true_allowed)) == 0)
			path_ok = true;

		free(true_allowed);

	} while(*++allowed && !path_ok);

	free(true_path);

	if(!path_ok)
		log_exit(4, "Error, path %s is not in the allowed-paths", path);
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

	if(pw->pw_gid != mygid)
		if(setgid(pw->pw_gid) != 0)
			log_exit_perror(4, "becoming group %s (%d)", gr->gr_name, pw->pw_gid);

	if(setuid(pw->pw_uid) != 0)
		log_exit_perror(4, "becoming user %s", username);
}


void die_unless_user(const char *user)
{
	struct passwd *pw = NULL;
	uid_t my_uid;

	my_uid = getuid();

	errno = 0;
	pw = getpwuid(my_uid);
	if(pw == NULL)
		log_exit(1, "my own UID (%d) not found!?", my_uid);
	else if(errno != 0)
		log_exit_perror(1, "getpwnam");

	if(strcmp(pw->pw_name, user) != 0)
		log_exit(4, "Error: must be run as %s user", user);
}
