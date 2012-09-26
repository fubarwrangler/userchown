#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

/* Gets errno EDQUOT on write() or close() */

#include "permissions.h"
#include "exitcodes.h"
#include "config.h"
#include "file.h"
#include "util.h"

#ifndef CONFIG_PATH
	#define CONFIG_PATH  "/etc/userchown.cfg"
#endif

static void usage(const char *name)
{
	fprintf(stderr,
"Usage: %s -u USER INPUT DESTINATION\n\
    Options:\n\
        -u  user to become for transfer\n\
        -h  print this help message\n\n\
    Arguments:\n\
        INPUT - input file to read, must be\n\
        DESTINATION  optional output destination, defaults to stdout\n\n",
    name);

}

int main(int argc, char *argv[])
{
	char *user = NULL;
	char *input = NULL;
	char *output = NULL;
	struct config cfg;
	int c;

	while((c=getopt(argc, argv, "hu:")) != -1)	{
		switch(c)	{
			case 'u':
				user = optarg;
				break;
			case 'h':
				usage(argv[0]);
				exit(EXIT_SUCCESS);
			case '?':
				if(strchr("u", optopt) == NULL)
					fprintf(stderr,
						"Unknown option -%c encountered\n", optopt);
				else
					fprintf(stderr,
						"Option -%c requires an argument\n", optopt);
				exit(EXIT_FAILURE);
			default:
				abort();
		}
	}
	if(user == NULL)	{
		usage(argv[0]);
		exit(1);
	}

	if(optind + 2 == argc)	{
		input = argv[optind];
		output = argv[optind + 1];
	} else	{
		fprintf(stderr, "Invalid number of arguments\n\n");
		usage(argv[0]);
		exit(1);
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

	return 0;
}
