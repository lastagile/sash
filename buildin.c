#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include "wash.h"
#include "buildin.h"


int cd_handler(char * arg[ARG_MAX])
{
	if(NULL == arg[1])
		return -1;
	return chdir(arg[1]);
}
