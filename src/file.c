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
	 * mount for each one.
	 */
	ok_dir = scan_paths(path, allowed);
	if(ok_dir != NULL)	debug("Path %s in allowed paths", path);

	/* Expand the directory-portion of path */
	pathsplit(path, &output_dir, NULL);
	if((true_path = expand_dir(output_dir)) == NULL)
		log_exit_perror(PATHRESOLV_ERROR,
						"Error expanding output path %s", path);

	/* If we didn't match the first time, try matching the expanded path */
	if(ok_dir == NULL)	{
		debug("Path %s unexpanded not in allowed paths", path);
		if((ok_dir = scan_paths(true_path, allowed)) == NULL)	{
			log_exit(PATHPERM_ERROR,
					"Error, path %s expands to %s, not in allowed paths",
					path, true_path);
		}
		debug("Path %s -> %s in allowed", path, true_path);
	}

	/* Expand the allowed-dir that matched above to detect symlink-escape */
	if((true_allowed = expand_dir(ok_dir)) == NULL)
		log_exit_perror(PATHRESOLV_ERROR,
						"Error expanding config-file path %s", true_allowed);
	if(strncmp(true_path, true_allowed, strlen(true_allowed)) != 0)	{
		debug("Expanded path %s doesn't match expanded allow-match %s",
			  true_path, true_allowed);
		log_exit(PATHPERM_ERROR,
				"Error: '%s' expands to '%s' which is not in the allowed-paths",
				output_dir, true_path);
	}

	free(output_dir);
	free(true_allowed);
	free(true_path);
}

static int do_copy(int infd, int outfd, blksize_t bufsize, int *err)
{
	ssize_t written, this_write;
	ssize_t bytes_read;
	char *buf = NULL;
	int rv = 0;

	*err = 0;
	buf = safemalloc(bufsize, "copy buf");

	while(true)	{
		errno = 0;
		bytes_read = read(infd, buf, bufsize);
		if(bytes_read < 0)	{
			if(errno == EINTR)
				continue;
			*err |= READ_ERROR;
			rv = errno;
			goto out;
		} else if (bytes_read == 0) {
			break;
		}

		written = 0;
		while(written < bytes_read)	{
			errno = 0;
			this_write = write(outfd, buf + written, bytes_read - written);
			if(this_write < 0)	{
				if(errno == EINTR)
					continue;
				*err |= WRITE_ERROR;
				rv = errno;
				goto out;
			}
			written += this_write;
		}
	}

out:
	free(buf);
	return rv;
}


/* If output is a directory, append input filename onto output path, otherwise
 * just return output. In either case, strip multiple '/' characters out of
 * the pathname. If @require_dir is set to
 */
static char *normalize_output(const char *input, const char *output)
{
	char *fixed_output;
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
	char *proper_output;
	int errval, err_type;
	int infd, outfd;
	blksize_t blocksize;
	struct stat sb;

	if(input == NULL || output == NULL)
		log_exit(INTERNAL_ERROR,
				 "BUG!: copy_file called with NULL input or output");

	debug("opening input: %s", input);
	if((infd = open(input, O_RDONLY)) < 0)
		log_exit_perror(FILEPERM_ERROR, "open input %s", input);

	if((proper_output = normalize_output(input, output)) == NULL)
		log_exit(INTERNAL_ERROR, "BUG: Error normalizing output!?!");

	debug("opening output: %s", proper_output);
	if((outfd = open(proper_output, O_CREAT|O_WRONLY, 0644)) < 0)
		log_exit_perror(FILEPERM_ERROR, "open output '%s'", proper_output);

	free(proper_output);

	if(fstat(outfd, &sb) != 0)
		log_exit_perror(IO_ERROR, "fstat() opened output file?");

	/* Blocksize up to 64k then stop at that */
	blocksize = (sb.st_blksize > 1024 * 64) ? 1024 * 64 : sb.st_blksize;
	debug("FS blocksize is %ld, using %ld to copy %ld bytes",
		  sb.st_blksize, blocksize, sb.st_size);

	if((errval = do_copy(infd, outfd, blocksize, &err_type)) != 0)	{
		if (err_type & READ_ERROR)
			log_exit(IO_ERROR, "Read error: %s", strerror(errval));
		else if (err_type & WRITE_ERROR)	{
			log_exit(scan_output_errors(errval),
					 "Write error: %s", strerror(errval));
		}
	}
	debug("Copy done, closing input and output files");

	if(close(infd) < 0)
		log_exit_perror(IO_ERROR, "close input file!?");

	errno = 0;
	if((errval = close(outfd)) < 0)
		log_exit(scan_output_errors(errno),
				 "Close error: %s", strerror(errno));

	return 0;
}
