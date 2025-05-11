#include <stdlib.h>
#include <stdio.h>
#ifndef EXECUTOR_H
#define EXECUTOR_H
int is_executable(char *path);
char *find_in_path(char *command);
void execute_cmd(char **argv, int options, int *fd);
void redirect_output(char *str);
void execute_pipe(char *str);
#endif
