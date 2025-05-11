#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <ctype.h>

#include "parser.h"
#include "executor.h"

#include <readline/readline.h>
#include <readline/history.h>

#define history_size 10
static char *line_read = (char *)NULL;
char *rl_gets();

int main()
{
    // Flush after every printf
    setbuf(stdout, NULL); // so jha bhi printf hoga rather than storing the value to a buffer it will be flushed directly to stdout.

    char *input = NULL;
    // ok now this stores the executed commands in it.
    while (1)
    {
        input = rl_gets(); //\n removed

        char *clean_input;

        if (strcmp(input, "") != 0)
        {

            clean_input = process_input(input);
        }
        else
        {
            continue;
        }

        char *inp_dup = strdup(clean_input);
        char *token = strtok(inp_dup, " "); // returns the first word. before space
        if (strchr(clean_input, '|') != NULL)
        {
            printf("ok found pipe\n");
            execute_pipe(clean_input);
            continue;
        }
        else if (strchr(clean_input, '>') != NULL)
        {
            printf("ok found redirect\n");
            redirect_output(clean_input);
            continue; // don't need to do anything else after redirecting the output.
        }

        if (strcmp(token, "echo") == 0)
        {
            printf("%s\n", clean_input + 5);
        }
        else if (strcmp(token, "type") == 0)
        {
            token = strtok(NULL, " ");
            if (token == NULL)
                continue;
            if (strcmp(token, "type") == 0 || strcmp(token, "cd") == 0 || strcmp(token, "pwd") == 0 || strcmp(token, "echo") == 0 || strcmp(token, "exit") == 0)
            {
                printf("%s is a shell builtin \n", token);
            }
            else if (token != NULL)
            {
                char *path = find_in_path(token);
                if (path)
                {
                    printf("%s is %s\n", token, path);
                }
                else
                {
                    printf("%s: not found\n", token);
                }
            }
            free(inp_dup);
        }
        else if (strcmp(token, "exit") == 0)
        {
            exit(0);
        }
        else
        { // the command needs to be looked for in the system and then needs to be executed in a different process
            char *inp_dup = strdup(clean_input);
            char **args = parse_command(inp_dup);
            char *cmd = args[0];
            char *path = find_in_path(cmd);
            if (path != NULL)
            {
                execute_cmd(args, 0, NULL);
            }
            else
            {
                printf("command not found\n");
            }
            free(args);
            free(inp_dup);
        }

        free(clean_input);
    }

    return 0;
}
char *rl_gets()
{
    // If the buffer has already been allocated, return the memory
    // to the free pool.
    if (line_read)
    {
        free(line_read);
        line_read = (char *)NULL;
    }
    line_read = readline("42>");
    if (line_read && *line_read)
    {
        add_history(line_read);
    }
    // upon hitting the arrow keys like dup and down i canm  basically traverse through the command history
    return (line_read);
}
// gcc -o main main.c parser.c executor.c -lreadline or make a makefile
