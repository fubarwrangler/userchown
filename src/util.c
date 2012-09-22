#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <time.h>


static inline void log_msg_init(void)
{
	char p[24];
	time_t t = time(NULL);

	if(strftime(p, 33, "%m/%d %X", localtime(&t)) > 0)
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

void log(const char *fmt, ...)
{
	va_list ap;
	log_msg_init();
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fputc('\n', stderr);
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

void *safemalloc(size_t size, const char *fail)
{
	void *p = malloc(size);
	if(p == NULL)
		log_exit(1, "safemalloc error: %s", fail);
	memset(p, 0, size);
	return p;
}


void saferealloc(void **p, size_t new_size, const char *fail)
{
	void *tmp = realloc(*p, new_size);
	if(tmp == NULL)
		log_exit(3, "realloc failed: %s", fail);
	*p = tmp;
}

char *safestrdup(const char *str, const char *fail)
{
	char *p = strdup(str);
	if(p == NULL)
		log_exit(3, "strdup failed: %s", fail);
	return p;
}

void pathsplit(const char *path, char **dir, char **file)
{
	char *d, *f;

	d = safestrdup(path, "pathsplit");
	f = safestrdup(path, "pathsplit");
	if(strchr(path, '/') == NULL)	{
		*dir = safestrdup(".", "pathsplit"), *file = f;
		return;
	}


}
