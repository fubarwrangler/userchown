#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdbool.h>

#include "exitcodes.h"
#include "config.h"
#include "file.h"
#include "user.h"
#include "util.h"

#ifndef CONFIG_PATH
	#define CONFIG_PATH  "/etc/userchown.cfg"
#endif

int _debug = 0;

static void usage(const char *name)
{
	fprintf(stderr,
"Usage: %s -u USER INPUT DESTINATION\n\
\n\
Copy a file as the user given to the destination provided.\n\
\n\
Options:\n\
  -u  user to become for transfer\n\
  -d  print debug messages\n\
  -h  print this help message\n\n\
  INPUT - input file to read, must be\n\
  DESTINATION - destination, if a directory (ends with '/') then preserve\n\
                the filename (behaves just like 'cp' command).\n\n",
    name);

}

int main(int argc, char *argv[])
{
	char *user = NULL;
	char *input = NULL;
	char *output = NULL;
	struct config cfg;
	int c;

	while((c=getopt(argc, argv, "hdu:")) != -1)	{
		switch(c)	{
			case 'u':
				user = optarg;
				break;
			case 'h':
				usage(argv[0]);
				exit(NO_ERROR);
			case 'd':
				_debug = 1;
				break;
			case '?':
				if(strchr("u", optopt) == NULL)
					fprintf(stderr,
						"Unknown option -%c encountered\n", optopt);
				else
					fprintf(stderr,
						"Option -%c requires an argument\n", optopt);
				exit(USAGE_ERROR);
			default:
				abort();
		}
	}
	if(optind + 2 == argc)	{
		input = argv[optind];
		output = argv[optind + 1];
	} else	{
		fprintf(stderr, "Invalid number of arguments\n"
			    "\tRerun with -h to see usage\n");
		exit(USAGE_ERROR);
	}

	if(user == NULL)	{
		fprintf(stderr,
				"Error, user must be supplied with -u <user> argument\n"
				"\tRerun with -h to see usage-details\n");
		exit(USAGE_ERROR);
	}

	/* All of these functions exit the program unless everything is A-OK */
	read_config(CONFIG_PATH, &cfg);

	/* If the user running the program doesn't match the config-file, exit */
	die_unless_user(cfg.required_user);

	/* If the output path isn't in the list of allowed outputs, exit */
	validate_output(output, cfg.allowed_paths);

	/* Try to become target user iff. they are a member of the right group */
	if_valid_become(user, cfg.required_group);

	destroy_config(&cfg);

	/* Do the actual copy, failing on any error condition */
	copy_file(input, output);

	debug("Finished, exit OK");
	return NO_ERROR;
}
