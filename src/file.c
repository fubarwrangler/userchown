#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "util.h"
#include "file.h"
#include "exitcodes.h"


static int do_copy(int infd, int outfd, size_t bufsize, int *err)
{
	ssize_t written, this_write;
	ssize_t bytes_read;
	char *buf;

	*err = 0;
	buf = safemalloc(bufsize, "copy buf");

	while(true)	{
		bytes_read = read(infd, buf, bufsize);
		if(errno != 0 || bytes_read <= 0)	{
			if(errno == EINTR)
				continue;
			if(errno != 0)	{
				*err |= READ_ERROR;
				free(buf);
				return errno;
			}
			break;
		}

		written = 0;
		while(written < bytes_read)	{

			this_write = write(outfd, buf + written, bytes_read - written);

			if(errno != 0 || this_write < 0)	{
				if(errno == EINTR) {
					continue;
				} else if (errno != 0)	{
					*err |= WRITE_ERROR;
					free(buf);
					return errno;
				}
			}
			written += this_write;
		}
	}

	free(buf);
	return 0;
}

/* If output is a directory, append input filename onto output path, otherwisew
 * just return output. In either case, strip multiple '/' characters out of
 * the pathname.
 */
static char *normalize_output(const char *input, const char *output)
{
	char *fixed_output = NULL;
	char *input_file;
	char *output_file;
	char *output_path;


	pathsplit(input, NULL, &input_file);
	pathsplit(output, &output_path, &output_file);

	if(*output_file == '\0')
		fixed_output = pathjoin(output_path, input_file);
	else
		fixed_output = pathjoin(output_path, output_file);

	free(output_path);
	free(output_file);
	free(input_file);

	return fixed_output;
}

static int scan_output_errors(int err)
{
	switch(err)	{
		case 0:
			return NO_ERROR;
		case EDQUOT:
			return QUOTA_ERROR;
		default:
			return IO_ERROR;
	}
}

int copy_file(const char *input, const char *output)
{
	int errval, err_type;
	int infd, outfd;
	char *proper_output;

	if((infd = open(input, O_RDONLY)) < 0)
		log_exit_perror(FILEPERM_ERROR, "open input %s", input);

	if((proper_output = normalize_output(input, output)) == NULL)
		log_exit(INTERNAL_ERROR, "Unspecified error normalizing path?");

	if((outfd = open(proper_output, O_CREAT|O_WRONLY, 0644)) < 0)
		log_exit_perror(FILEPERM_ERROR, "open output '%s'", proper_output);

	free(proper_output);

	if((errval = do_copy(infd, outfd, 4096, &err_type)) != 0)	{
		if (err_type & READ_ERROR)
			log_exit(IO_ERROR, "Read error: %s", strerror(errval));
		else if (err_type & WRITE_ERROR)	{
			log_exit(scan_output_errors(errval),
					 "Write error: %s", strerror(errval));
		}
	}

	if(close(infd) < 0)
		log_exit_perror(IO_ERROR, "close input file!?");

	errno = 0;
	if((errval = close(outfd)) < 0)
		log_exit(scan_output_errors(errno),
				 "Close error: %s", strerror(errno));

	return 0;
}
