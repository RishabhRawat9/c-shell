#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <ctype.h>
#include "parser.h"
#include "executor.h"

int main()
{
    // Flush after every printf
    setbuf(stdout, NULL); // so jha bhi printf hoga rather than storing the value to a buffer it will be flushed directly to stdout.

    while (1)
    {
        printf("42> ");
        // Wait for user input
        char input[100]; // this memeory allocated on stack
        fgets(input, 100, stdin);

        input[strcspn(input, "\n")] = 0;
        char *clean_input;

        clean_input = process_input(input);
        char *inp_dup = strdup(clean_input);
        char *token = strtok(inp_dup, " "); // returns the first word. before space

        // just looking for some of the builtins.
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
                execute_cmd(path, args, 1);
            }
            free(args);
            free(inp_dup);
        }
    }
    return 0;
}
// gcc -o main main.c parser.c executor.c or make a makefile
