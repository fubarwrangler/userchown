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

bool read_config(const char *cfgfile, char ***paths)
{
	FILE *fp;
	char line[1024];
	size_t cur_size = 0, alloc_size = 4;
	char **list;

	if((fp = fopen(cfgfile, "r")) == NULL)
		log_exit_perror(1, "open cfgfile");


	if((list = malloc(10 * sizeof(char *))) == NULL)
		log_exit_perror(1, "malloc pathlist");


	while(fgets(line, 1023, fp) != NULL)	{

		if(ferror(fp))	{
			perror("cfgread fgets");
			return false;
		}

		if(filter_line(line) == false)
			continue;

		if(cur_size + 2 > alloc_size)	{
			void *tmp = realloc(list, (alloc_size * 2) * sizeof(char *));
			printf("Reallocating %d -> %d", alloc_size, alloc_size * 2);
			if(tmp == NULL)
				log_exit_perror(3, "realloc");
			list = tmp;
			alloc_size *= 2;
		}
		if((list[cur_size] = strdup(line)) == NULL)
			log_exit_perror(1, "malloc path");

		cur_size++;

	}
	list[cur_size] = NULL;

	*paths = list;

	return true;
}

