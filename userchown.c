#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>


/* Gets errno EDQUOT on write() or close() */

#include "config.h"


void log_exit_perror(int code, const char *fmt, ...)
{
    char buf[1024];
    va_list ap;

    log_msg_init();
    va_start(ap, fmt);
    vsnprintf(buf, 1022, fmt, ap);
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


int main(int argc, char const *argv[])
{
	char **p, **q;
	read_config(&p, "")
	return 0;
}
