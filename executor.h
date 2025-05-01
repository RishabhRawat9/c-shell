#include <stdlib.h>
#include <stdio.h>
#ifndef EXECUTOR_H
#define EXECUTOR_H
int is_executable(char *path);
char *find_in_path(char *command);
void execute_cmd(char *path, char **argv, int options); // what command needs to be executed, what are its args, options is for redirecting to diffent places

#endif
