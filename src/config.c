#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "config.h"
#include "util.h"

#define CFG_BUFSIZE 2048

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

/* read_config()
 *
 * read file @cfgfile into an array of strings pointed to by @paths.
 *
 * Strips leading and trailing whitespace from the lines and ignores blank
 * lines or comments that start with '#'
 */
void read_config(const char *cfgfile, char ***paths)
{
	FILE *fp;
	char line[CFG_BUFSIZE];
	size_t cur_size = 0, alloc_size = 4;
	char **list;

	if((fp = fopen(cfgfile, "r")) == NULL)
		log_exit_perror(1, "open cfgfile");

	list = safemalloc(alloc_size * sizeof(char *), "pathlist");

	while(fgets(line, CFG_BUFSIZE - 1, fp) != NULL)	{

		if(ferror(fp))
			log_exit_perror(1, "cfgread");

		if(filter_line(line) == false)
			continue;

		if(cur_size + 2 > alloc_size)	{
			saferealloc((void **)&list, (alloc_size * 2) * sizeof(char *), "pathlist");
			alloc_size *= 2;
		}
		if((list[cur_size] = strdup(line)) == NULL)
			log_exit_perror(1, "malloc path");

		cur_size++;

	}
	list[cur_size] = NULL;

	*paths = list;
}

