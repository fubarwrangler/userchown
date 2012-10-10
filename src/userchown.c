#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>

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
"Usage: %s -u USER INPUT(s) DESTINATION\n\
\n\
Copy a file as the user given to the destination provided.\n\
\n\
Options:\n\
  -u  user to become for transfer\n\
  -d  print debug messages\n\
  -h  print this help message\n\n\
  INPUT - input file(s) to read, if multiple are passed then DESTINATION\n\
          must be a directory.\n\
  DESTINATION - destination, if a directory (ends with '/') then preserve\n\
                the filename (behaves just like 'cp' command).\n\n",
    name);

}

int main(int argc, char *argv[])
{
	char *user = NULL;
	char *input = NULL;
	char *output = NULL;
	int input_location = 0;
	bool multiple_input = false;
	bool one_passed = false;
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
	debug("optind: %d, argc: %d", optind, argc);
	if(optind + 2 == argc)	{
		input = argv[optind];
		output = argv[optind + 1];
		multiple_input = false;
	} else if (optind + 2 < argc)	{
		input_location = optind;
		output = argv[argc - 1];
		multiple_input = true;
	} else  {
		fprintf(stderr, "Invalid number of arguments\n");
		usage(argv[0]);
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
	if(!multiple_input)	{
		copy_file(input, output);
	} else {
		struct stat sb;
		char *p;
		int i;

		if(!is_directory(output))
			log_exit(USAGE_ERROR, "Error: destination %s must be a directory");

		debug("Multiple inputs, %d files to %s", argc - input_location - 1,
			  output);
		for(i = input_location; i < argc - 1; i++)	{
			p = argv[i];

			if(stat(p, &sb) < 0)
				log_exit_perror(FILEPERM_ERROR, "stat input: %s", p);

			switch(sb.st_mode & S_IFMT)	{
				case S_IFREG:
					debug("**************  Copy file %d  ****************",
						  i - input_location + 1);
					copy_file(p, output);
					one_passed = true;
					break;
				case S_IFDIR:
					log_msg("%s is a directory, skipping", p);	break;
				default:
					log_msg("%s is not a regular file, skip", p);	break;
			}
		}
	}

	if(multiple_input && !one_passed)	{
		debug("No files copied, exit NO_ACTION_ERROR");
		return NO_ACTION_ERROR;
	} else {
		debug("Finished, exit OK");
		return NO_ERROR;
	}
}
