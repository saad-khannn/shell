#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#ifndef SHELL_H
#define SHELL_H

#define BUILTINS 8
#define T_BUFSIZE 256
#define DELIM " \t\r\n\a"

int cmd_cd(char **);
int cmd_clr();
int cmd_dir(char **);
int cmd_echo(char **);
int cmd_environ();
int cmd_help();
int cmd_pause();
int cmd_quit(char **param);

void shell(void);
char *get_input(void);
char **read_input(char *param);
int exec(char **param);

#endif
