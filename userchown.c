#include <stdio.h>
#include <stdlib.h>

/* Gets errno EDQUOT on write() or close() */

#include "config.h"
#include "file.h"



int main(int argc, char const *argv[])
{
	char **p, **q;

	if(argc < 3)
		log_exit(2, "Usage: %s intput output", argv[0]);

	copy_file(argv[1], argv[2]);

	return 0;
}
