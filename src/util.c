#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <time.h>

#include "util.h"
#include "exitcodes.h"

static inline void log_msg_init(void)
{
	char p[64];
	time_t t = time(NULL);

	if(strftime(p, 63, "%m/%d %X", localtime(&t)) > 0)
		fprintf(stderr, "%s phnxchown: ", p);
}


void log_exit_perror(int code, const char *fmt, ...)
{
	char buf[2048];
	va_list ap;

	log_msg_init();
	va_start(ap, fmt);
	vsnprintf(buf, 2046, fmt, ap);
	va_end(ap);
	perror(buf);
	exit(code);
}

void log_exit(int code, const char *fmt, ...)
{
	va_list ap;
	log_msg_init();
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fputc('\n', stderr);
	exit(code);
}

void debug(const char *fmt, ...)
{
	va_list ap;
	if(_debug != 0)	{
		log_msg_init();
		va_start(ap, fmt);
		vfprintf(stderr, fmt, ap);
		va_end(ap);
		fputc('\n', stderr);
	}
}

/* Returns zeroed block */
void *safemalloc(size_t size, const char *fail)
{
	void *p = malloc(size);
	if(p == NULL)
		log_exit(MEMORY_ERROR, "safemalloc error: %s", fail);
	memset(p, 0, size);
	return p;
}


void saferealloc(void **p, size_t new_size, const char *fail)
{
	void *tmp = realloc(*p, new_size);
	if(tmp == NULL)
		log_exit(MEMORY_ERROR, "realloc failed: %s", fail);
	*p = tmp;
}

char *safestrdup(const char *str, const char *fail)
{
	char *p = strdup(str);
	if(p == NULL)
		log_exit(MEMORY_ERROR, "strdup failed: %s", fail);
	return p;
}

/* remove runs of char 'c' from string *str */
static void rlc(char *str, char c)
{
	char *q;

	if(strlen(str) < 2)
		return;

	q = str;
	while(*q)	{
		q++;
		/* q runs ahead skipping over runs of @c */
		while(*str == *q && *q == c)
			q++;
		str++;
		/* copy (possibly different) q into earlier location */
		*str = *q;
	}
}

/* Calls realpath but appends a trailing '/' if the path has none already.
 * Because of the above, it expects @dir to be a directory, not a file!
 */
char *expand_dir(const char *dir)
{
	size_t exp_len;
	char *expanded = NULL;

	expanded = realpath(dir, NULL);

	if(expanded != NULL && dir[strlen(dir)] != '/')	{
		exp_len  = strlen(expanded);
		saferealloc((void *)&expanded, exp_len + 2, "expand_path realloc");
		expanded[exp_len ] = '/';
		expanded[exp_len + 1] = '\0';
	}
	return expanded;
}

/* pathsplit() - normalize and split a path into directory and file components
 *    @path - the full path to split up
 *    @file - place for filename component, or NULL to ignore this part
 *    @dir  - place for directory component, or NULL to ignore this part
 *
 * If either @dir or @file is NULL those components will not be allocated
 * and populated. The path will be normalized: runs of multiple '/' characters
 * will be stripped down to one.
 *
 * API: new storage will be allocated for @dir and @file
 *
 */
void pathsplit(const char *path, char **dir, char **file)
{
	char *normal_path;

	/* So we don't modify *path */
	normal_path = safestrdup(path, "pathsplit");

	/* Remove runs of '/' from input */
	rlc(normal_path, '/');

	/* If we don't have a '/' */
	if(strchr(normal_path, '/') == NULL) {
		/* If we're just .. or ., return empty file */
		if(strcmp(".", normal_path) == 0 || strcmp("..", normal_path) == 0)	{
			if(dir != NULL)
				*dir = safestrdup(normal_path, "pathsplit");
			if(file != NULL)
				*file = safestrdup("", "pathsplit");
		/* If we're a filename, use '.' as current directory */
		} else {
			if(dir != NULL)
				*dir = safestrdup(".", "pathsplit");
			if(file != NULL)
				*file = safestrdup(normal_path, "pathsplit");
		}
	} else {
		size_t index, pathlen;

		pathlen = strlen(normal_path);

		/* Position of last '/' character */
		index = (size_t)(strrchr(normal_path, '/') - normal_path);

		if(dir != NULL)	{
			/* If '/' at the beginning, just '/' is dir */
			if(index == 0)	{
				*dir = safestrdup("/", "pathsplit");
			/* Else path up to last '/' is dir (move `index` bytes into dir) */
			} else {
				*dir = safemalloc(index + 1, "pathsplit");
				memmove(*dir, normal_path, index);
				*(*dir + index) = '\0';
			}
		}
		if(file != NULL)	{
			/* File is always path from index onward */
			*file = safemalloc(pathlen - index + 1, "pathsplit");
			memmove(*file, normal_path + index + 1, pathlen - index);
			*(*file + (pathlen - index)) = '\0';
		}
	}
	free(normal_path);
}

/* pathjoin() : join dir and file into a full path
 *    @dir -  directory, can be relative or absolute (start with '/')
 *    @file - filename or relative directory (must not start with '/')
 *
 * Returns: new string with full path of dir/file
 *
 * Will strip out repetitions of '/' in either argument. If file starts with
 * a '/' NULL is returned.
 *
 * API: new storage returned - should be free'd by the user
 */
char *pathjoin(const char *dir, const char *file)
{
	size_t dlen, flen;
	char *d, *f;
	char *p;

	/* assert(file != NULL && dir != NULL); */

	if(*file == '/')
		return NULL;

	d = safestrdup(dir, "pathjoin");
	rlc(d, '/');
	f = safestrdup(file, "pathjoin");
	rlc(f, '/');

	dlen = strlen(d);
	flen = strlen(f);

	p = safemalloc(dlen + flen + 2, "pathjoin");
	memmove(p, d, dlen);

	if(d[dlen-1] != '/')
		*(p + dlen++) = '/';

	memmove(p + dlen, f, flen);

	free(d), free(f);
	return p;
}
