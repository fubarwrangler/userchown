#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

/* Gets errno EDQUOT on write() or close() */

#include "permissions.h"
#include "config.h"
#include "file.h"
#include "util.h"

#define REQUIRED_GROUPID 1001 /* rhphenix GID */
#define CONFIG_PATH  "/etc/phnxchown.cfg"


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
	char *input = NULL, *output = NULL;
	struct config *cfg = safemalloc(sizeof(struct config), "cfgstruct");
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

	read_config(CONFIG_PATH, cfg);

	die_unless_user(cfg->required_user);

	if(!file_allowed(output, cfg->allowed_paths))
		log_exit(2, "Output file %s not in list of allowable outputs", output);

	if_valid_become(user, cfg->required_group);

	destroy_config(cfg);

	copy_file(input, output);

	return 0;
}
