#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

#include "util.h"
#include "exitcodes.h"


gid_t lookup_gid(const char *groupname)
{
	int errval;
	gid_t gid;
	size_t buflen = 1024;
	struct group gr;
	struct group *grptr;
	char *buf = NULL;

	while(true)	{
		saferealloc((void *)&buf, buflen, "gid_lookup");
		errno = 0;
		errval = getgrnam_r(groupname, &gr, buf, buflen, &grptr);
		if(grptr != NULL)	{
			gid = grptr->gr_gid;
		} else if (errval == ERANGE)	{
			buflen = buflen * 2;
			continue;
		} else {
			log_exit(LDAP_ERROR,
					 "Error looking up group %s: %s",
					 groupname, strerror(errval));
		}
		break;
	}
	free(buf);
	return gid;

}

void if_valid_become(const char *username, const char *required_group)
{
	struct passwd *pw = NULL;
	struct group *gr = NULL;
	gid_t mygid = getgid();

	errno = 0;
	pw = getpwnam(username);
	if(pw == NULL && errno == 0)
		log_exit(USERABSENT_ERROR, "user %s not found", username);
	else if(errno != 0)
		log_exit_perror(LDAP_ERROR, "getpwnam");

	errno = 0;
	gr = getgrnam(required_group);
	if(gr == NULL && errno == 0)
		log_exit(USERABSENT_ERROR, "required-group %s not found",
				 required_group);
	else if(errno != 0)
		log_exit_perror(LDAP_ERROR, "getgrname");

	if(pw->pw_gid != gr->gr_gid)
		log_exit(USERPERM_ERROR,
				 "%s's group id is %d, but needs to be %d (%s)",
				 username, pw->pw_gid, gr->gr_gid, required_group);

	if(pw->pw_gid != mygid)
		if(setgid(pw->pw_gid) != 0)
			log_exit_perror(USERPERM_ERROR, "becoming group %s (%d)",
							gr->gr_name, pw->pw_gid);

	if(setuid(pw->pw_uid) != 0)
		log_exit_perror(USERPERM_ERROR, "becoming user %s", username);
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
	if(pw == NULL && errno == 0)
		log_exit(INTERNAL_ERROR, "my own UID (%d) not found!?", my_uid);
	else if(errno != 0)
		log_exit_perror(LDAP_ERROR, "getpwnam");

	if(strcmp(pw->pw_name, user) != 0)
		log_exit(USERPERM_ERROR, "Error: must be run as %s user", user);
}
