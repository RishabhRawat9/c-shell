#include <stdlib.h>
#include <stdio.h>
#ifndef PARSER_H
#define PARSER_H

char *process_input(char *input);
char **parse_command(char *input); // tokenizes the input into main command and args, then returns the arr

#endif
