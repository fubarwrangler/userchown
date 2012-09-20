#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "config.h"
#include "userchown.h"


/* Strip leading whitespace + newline, return false for comments or blank */
static bool filter_line(char *raw)
{
	size_t l_white = 0;
	size_t len = strlen(raw);

	if(len > 1) {
		/* How many whitespace chars start the string? */
		l_white = strspn(raw, "\t ");

		/* Move the non-whitespace part to the begenning of the string */
		if(l_white > 0)
			memmove(raw, (raw + l_white), len - l_white);

		/* Strip off newline and adjust for removal of leading whitespace */
		raw[len - l_white - 1] = '\0';
	}
	/* Skip comments and blank lines */
	return !(*raw == '#' || *raw == ';' || len < l_white + 2);
}

/* Set up environment for image-starting */
char **set_up_condor_env(char *ok_environ)
{
	char **new_env = NULL;
	char **tmp = NULL;
	char **p = environ;

	size_t cur_size = 0;
	size_t alloc_size = 4;

	if((new_env = malloc(alloc_size * sizeof(char *))) == NULL)
		log_exit_perror(3, "malloc");

	while(*p != NULL)
	{
		char *found = strstr(*p, ok_environ);

		/* Make sure first '=' sign is after the address where *ok_env is */
		if(found != NULL && strchr(*p, '=') > found)	{
			if(cur_size + 2 > alloc_size)	{
				tmp = realloc(new_env, (alloc_size * 2) * sizeof(char *));
				if(tmp == NULL)
					log_exit_perror(3, "realloc");
				new_env = tmp;
				alloc_size *= 2;
			}
			new_env[cur_size] = *p;
			cur_size++;
		}
		p++;
	}

	new_env[cur_size] = NULL;

	return new_env;
}

bool read_config(const char *cfgfile, char ***paths)
{
	FILE *fp;
	char buf[1024];
	char *p, **list;

	if((fp = fopen(cfgfile, "r")) == NULL)
		log_exit_perror(1, "open cfgfile");


	if((list = malloc(10 * sizeof(char *))) == NULL)
		log_exit_perror(1, "malloc pathlist");


	while(fgets(line, 1023, fp) != NULL)	{
		char *p;

		if(ferror(fp))	{
			perror("cfgread fgets");
			return false;
		}

		if(filter_line(line) == false)
			continue;

	}

	return true;
}

