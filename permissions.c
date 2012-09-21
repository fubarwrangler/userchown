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

#define REQUIRED_GROUPID 31013 /* rhphenix GID */


void if_valid_become(const char *username)
{
	struct passwd *pw = NULL;

	pw = getpwnam(username);
	if(pw == NULL || errno)
		log_exit_perror(1, "getpwuid");

	if(pw->pw_gid != REQUIRED_GROUPID)
		log_exit(4, "%s's group id is %d, but need to be %d (rhphenix)",
			username, pw->pw_gid, REQUIRED_GROUPID);

	if(setuid(pw->pw_uid) != 0)
		log_exit_perror(4, "becoming user %s", username);

}

/* Called as anatrain user, check access permissions */
void path_ok(const char *path)
{
	char *abspath;
	char *cwd;
	struct stat s;
	bool rv = true;

	if(stat(path, &s) != 0)
		log_exit_perror(4, "stat file %s", path);

	abspath = realpath(path, NULL);
	if(abspath == NULL)
		log_exit_perror(4, "expand path %s", path);

	if((cwd = getcwd(NULL, 0)) == NULL)
		log_exit_perror(4, "getcwd?");

	if(strncmp(cwd, abspath, strlen(cwd)) != 0)
		log_exit(4, "path %s isn't under the current working directory", path);

	free(cwd);
	free(abspath);
}
