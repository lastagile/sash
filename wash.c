#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include "wash.h"
#include "buildin.h"

int parse_token(char *buf, cmd_t cmd[]);
int parse(char *buf, cmd_t * cmd);
int test_parse(cmd_t cmd[], int len);
void run_cmd(cmd_t cmd[], int len);

build_in_t handler[]={
	{"cd",cd_handler}
};

int main(int argc, char *argv[])
{
	char path[256];
	char buf[CMD_LINE];
	cmd_t cmd[PIPE_MAX + 1];
	int fd[PIPE_MAX][2];
	int j, i;
	int cmd_len;

	while (1) {
		char *p = getcwd(path,256);
		if(NULL == p)
			printf("wash$");
		else
			printf("%s $",path);
		fgets(buf, CMD_LINE, stdin);
		if(feof(stdin))
		{
			printf("\n");
			break;
		}
		if(strlen(buf) == 0)
			continue;
		buf[strlen(buf) - 1] = '\0';
		cmd_len = parse_token(buf, cmd);
		if(0 == cmd_len)
			continue;
		run_cmd(cmd,cmd_len);
	}

	return 0;
}

int run_buildin(cmd_t *cmd)
{
	int i;
	for(i = 0; i < sizeof(handler) / sizeof(build_in_t); i++)
	{
		if(0 == strcmp(handler[i].cmd,cmd->arg[0]))
		{
			(*handler[i].pfun)(cmd->arg);
			return 1;
		}
	}
	return 0;
}

void run_cmd(cmd_t cmd[], int cmd_len)
{
	int fd[PIPE_MAX][2];
	int j, i;
	int pipe_len;
	pid_t pid;

	pipe_len = cmd_len - 1;
	if (pipe_len > PIPE_MAX)
		return;
	if(0 == pipe_len && run_buildin(cmd))
		return;

	for (i = 0; i < pipe_len; ++i)
		pipe(fd[i]);

	for (i = 0; i < cmd_len; ++i)
		if ((pid = fork()) == 0)
			break;

	if (pid == 0) {
		if (pipe_len) {
			if (i == 0) {
				close(fd[i][0]);
				dup2(fd[i][1], 1);
				close(fd[i][1]);
				for (j = 1; j < pipe_len; ++j)
					close(fd[j][0]), close(fd[j][1]);
			} else if (i == pipe_len) {
				close(fd[i - 1][1]);
				dup2(fd[i - 1][0], 0);
				close(fd[i - 1][0]);
				for (j = 0; j < pipe_len - 1; ++j)
					close(fd[j][0]), close(fd[j][1]);
			} else {
				dup2(fd[i - 1][0], 0);
				close(fd[i][0]);
				dup2(fd[i][1], 1);
				close(fd[i][1]);
				for (j = 0; j < pipe_len; ++j) {
					if ((j != i - 1) || (j != i))
						close(fd[j][0]), close(fd[j][1]);
				}
			}
		}
		if (cmd[i].in) {
			int fd = open(cmd[i].in, O_RDONLY);
			dup2(fd, STDIN_FILENO);
			close(fd);
		}
		if (cmd[i].out) {
			int fd = open(cmd[i].out, O_RDWR | O_CREAT | O_TRUNC, 0644);
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		execvp(cmd[i].arg[0], cmd[i].arg);
		fprintf(stderr, "Failed exec\n");
		exit(127);
	}
	/* parent  */
	for (i = 0; i < pipe_len; ++i)
		close(fd[i][0]), close(fd[i][1]);
	for (i = 0; i < cmd_len; ++i)
		wait(NULL);

	return;
}

int parse_token(char *buf, cmd_t cmd[])
{
	int n = 0;
	char *save_p;
	char *p = strtok_r(buf, "|", &save_p);
	while (p != NULL) {
		parse(p, &cmd[n++]);
		p = strtok_r(NULL, "|", &save_p);
	}
	return n;
}

int test_parse(cmd_t cmd[], int len)
{
	int i;
	for (i = 0; i < len; ++i) {
		printf("cmd[%d]:", i);
		int j = 0;
		while (cmd[i].arg[j])
			printf(" %s", cmd[i].arg[j++]);
		if (cmd[i].in)
			printf("\tin:%s", cmd[i].in);
		if (cmd[i].out)
			printf("\tout:%s", cmd[i].out);
		printf("\n");
	}
	return 0;
}

int parse(char *buf, cmd_t * cmd)
{
	int i = 0;
	cmd->in = NULL;
	cmd->out = NULL;
	char *p = strtok(buf, " ");
	while (p) {
		if (*p == '<') {
			if (*(p + 1))
				cmd->in = p + 1;
			else
				cmd->in = strtok(NULL, " ");
		} else if (*p == '>') {
			if (*(p + 1))
				cmd->out = p + 1;
			else
				cmd->out = strtok(NULL, " ");
		} else
			cmd->arg[i++] = p;
		p = strtok(NULL, " ");
	}
	cmd->arg[i] = NULL;
	return 0;
}
