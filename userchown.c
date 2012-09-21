#include <stdio.h>
#include <stdlib.h>

/* Gets errno EDQUOT on write() or close() */

#include "permissions.h"
#include "config.h"
#include "file.h"
#include "util.h"



int main(int argc, char const *argv[])
{
	char *p;

	if(argc < 2)
		log_exit(2, "Usage: %s path", argv[0]);


	if(path_ok(argv[1]))
		puts("Yes\n");
	else
		puts("Error, not under cwd\n");

	return 0;
}
