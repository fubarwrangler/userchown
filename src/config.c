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

void destroy_config(struct config *cfg)
{
	char **p = cfg->allowed_paths;
	while(*p)
		free(*p++);
	free(cfg->allowed_paths);
	free(cfg->required_group);
	free(cfg);
}

void read_config(const char *cfgfile, struct config *cfg)
{
	FILE *fp;
	bool in_list = false;
	char line[CFG_BUFSIZE];
	size_t cur_size = 0, alloc_size = 4;
	char **list;

	if((fp = fopen(cfgfile, "r")) == NULL)
		log_exit_perror(1, "open cfgfile");

	list = safemalloc(alloc_size * sizeof(char *), "pathlist");
	cfg->required_group = NULL;

	while(fgets(line, CFG_BUFSIZE - 1, fp) != NULL)	{

		if(ferror(fp))
			log_exit_perror(1, "cfgread");

		if(filter_line(line) == false)
			continue;

		if(in_list)	{
			if(*line == '[')	{
				in_list = false;
				continue;
			}
			if(cur_size + 2 > alloc_size)	{
				alloc_size *= 2;
				saferealloc((void **)&list, alloc_size * sizeof(char *),
					"pathlist");
				alloc_size *= 2;
			}
			list[cur_size++] = safestrdup(line, "alloc-cfgline");
		} else {
			if(strcmp(line, "[allowed_paths]") == 0)
				in_list = true;
			if(strncmp(line, "required_group:", 14) == 0)	{
				char *p = line + 15;
				while(*p == ' ' || *p == '\t')
					p++;
				cfg->required_group = safestrdup(p, "cfg-line");
			}
		}
	}
	list[cur_size] = NULL;

	if(cfg->required_group == NULL)
		log_exit(1, "'required_group' not found in config file");
	if(list[0] == NULL)
		log_exit(1, "no allowed paths found in config");

	fclose(fp);

	cfg->allowed_paths = list;
}

