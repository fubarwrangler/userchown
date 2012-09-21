#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>

#include "util.h"
#include "file.h"


static int do_copy(int infd, int outfd, size_t bufsize, int *err)
{
	ssize_t written, this_write;
	ssize_t bytes_read;
	char *buf;

	*err = 0;
	buf = safemalloc(bufsize, "copy buf");

	while(1)	{
		bytes_read = read(infd, buf, bufsize);
		if(errno || bytes_read <= 0)	{
			if(errno == EINTR)
				continue;
			if(errno != 0)	{
				*err |= READ_ERROR;
				return errno;
			}
			break;
		}

		written = 0;
		while(written < bytes_read)	{

			this_write = write(outfd, buf + written, bytes_read - written);

			if(errno || this_write < 0)	{
				if(errno == EINTR) {
					continue;
				} else if (errno != 0)	{
					*err |= WRITE_ERROR;
					return errno;
				}
			}
			written += this_write;
		}
	}

	return 0;
}


/*
 *
 */
int copy_file(const char *input, const char *output)
{
	int infd, outfd, errval, err_type;

	if((infd = open(input, O_RDONLY)) < 0)
		log_exit_perror(2, "open input %s", input);

	if((outfd = open(output, O_CREAT|O_WRONLY, 0644)) < 0)
		log_exit_perror(2, "open output %s", output);

	if((errval = do_copy(infd, outfd, 4096, &err_type)) != 0)	{
		if (err_type & READ_ERROR)
			log_exit(2, "Read error: %s\n", strerror(errval));
		else if (err_type & WRITE_ERROR)
			log_exit(2, "Write error: %s\n", strerror(errval));
	}

	if(close(infd) < 0)
		log_exit_perror(2, "close input file!?");

	errno = 0;
	if((errval = close(outfd)) < 0)	{
		return errno;
	}
	return 0;
}
