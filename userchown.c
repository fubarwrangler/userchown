#include <stdio.h>
#include <stdlib.h>

/* Gets errno EDQUOT on write() or close() */

#include "config.h"



int main(int argc, char const *argv[])
{
	char **p, **q;
	read_config((argc > 1)? argv[1]: "include/config.h", &p);
	q = p;
	while(*q)
		puts(*q++);

	return 0;
}
