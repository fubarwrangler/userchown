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
#include "exitcodes.h"


/* Don't return unless the path is allowed as per the allowed-list */
void validate_output(const char *path, char **allowed)
{
	bool path_match = false;
	char *true_path;
	char *true_allowed;
	char *ok_allowed;
	char *output_dir;

	pathsplit(path, &output_dir, NULL);

	/* Expand all symbolic links and ../ references in the path */
	if((true_path = realpath(output_dir, NULL)) == NULL)
		log_exit_perror(2, "Error expanding output path %s", path);

	free(output_dir);

	/* The configured paths must have all known symlinks expanded already or
	 * this won't work. We don't expand all the configured paths because that
	 * would force a mount for each one. We expand after we've matched the
	 * first time to make sure that when we resolve the full allowed-path it
	 * doesn't contain symlinks outside of itself.
	 */
	do	{
		if(strncmp(true_path, *allowed, strlen(*allowed)) == 0)	{
			path_match = true;
			ok_allowed = *allowed;
		}
	} while(*++allowed && !path_match);

	if(!path_match)
		log_exit(4, "Error, path %s is not in the allowed-paths", path);

	if((true_allowed = realpath(ok_allowed, NULL)) == NULL)
		log_exit_perror(2, "Error expanding config-file path %s", true_allowed);


	if(strncmp(true_path, true_allowed, strlen(true_allowed)) != 0)
		log_exit(4, "Error, path %s is not in the allowed-paths", path);

	free(true_allowed);
	free(true_path);
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

	/* root is always OK */
	if(my_uid == 0)
		return;

	errno = 0;
	pw = getpwuid(my_uid);
	if(pw == NULL)
		log_exit(1, "my own UID (%d) not found!?", my_uid);
	else if(errno != 0)
		log_exit_perror(1, "getpwnam");

	if(strcmp(pw->pw_name, user) != 0)
		log_exit(4, "Error: must be run as %s user", user);
}
