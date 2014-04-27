#ifndef BUILDIN_H__
#define BUILDIN_H__
typedef int (* build_in_handler_t)(char * arg[ARG_MAX]);

typedef struct{
	const char *cmd;
	build_in_handler_t pfun;
} build_in_t;

int cd_handler(char * arg[ARG_MAX]);

#endif
