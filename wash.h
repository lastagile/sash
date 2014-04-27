#ifndef WASH_H__
#define WASH_H__
#define CMD_LINE 2048
#define PIPE_MAX 32
#define ARG_MAX 10

typedef struct {
	char *arg[ARG_MAX];
	char *in;
	char *out;
} cmd_t;

#endif
