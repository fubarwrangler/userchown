#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <time.h>


/* Gets errno EDQUOT on write() or close() */

#include "config.h"
#include "userchown.h"


static inline void log_msg_init(void)
{
    char p[24];
    time_t t = time(NULL);

    if(strftime(p, 33, "%m/%d %X", localtime(&t)) > 0)
        fprintf(stderr, "%s phnxchown: ", p);
}


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
	read_config(&p, "");
	return 0;
}
