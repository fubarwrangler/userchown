#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "util.h"
#include "config.h"
#include "exitcodes.h"

/* Return NULL unless path matches one of the allowed */
static char *scan_paths(const char *path, char **allowed)
{
	do	{
		if(strncmp(path, *allowed, strlen(*allowed)) == 0)
			break;
	} while(*++allowed);

	return *allowed;
}

/* Don't return unless the path is allowed as per the allowed-list */
void validate_output(const char *path, char **allowed)
{
	char *true_path;
	char *true_allowed;
	char *ok_dir;
	char *output_dir;


	/* We don't expand all the configured paths because that would force a
	 * mount for each one. We expand after we've matched the unexpanded paths
	 * the first time to make sure that when we resolve them both fully the
	 * destination didn't contain symlinks outside of itself.
	 */
	if((ok_dir = scan_paths(path, allowed)) == NULL)
		log_exit(PATHPERM_ERROR,
				 "Error, path %s is not in the allowed-paths", path);

	/* Expand the directory-portion of path */
	pathsplit(path, &output_dir, NULL);
	if((true_path = expand_dir(output_dir)) == NULL)
		log_exit_perror(PATHRESOLV_ERROR,
						"Error expanding output path %s", path);

	if((true_allowed = expand_dir(ok_dir)) == NULL)
		log_exit_perror(PATHRESOLV_ERROR,
						"Error expanding config-file path %s", true_allowed);

	/* Compare with expansion that we are still OK */
	if(strncmp(true_path, true_allowed, strlen(true_allowed)) != 0)	{
		/* Check again that true-path isn't listed in the allowed-list */
		if(scan_paths(true_path, allowed) == NULL)
			log_exit(PATHPERM_ERROR,
					"Error: '%s' expands to '%s' which is not in the allowed-paths",
					output_dir, true_path);
	}

	free(output_dir);
	free(true_allowed);
	free(true_path);
}
