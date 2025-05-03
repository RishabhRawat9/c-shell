#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <ctype.h>
#include "parser.h"

int is_executable(char *path) { return access(path, X_OK) == 0; }
char *find_in_path(char *command)
{
    char *path_env = getenv("PATH"); // path environment variable.
    if (path_env == NULL)
    {
        return NULL;
    }
    char *path_copy = strdup(path_env);
    char *dir = strtok(path_copy, ":");
    static char full_path[1024]; // it's a buffer in which we make the complete path
    // of the command we are searching for.
    while (dir != NULL)
    {
        // using snprintf i am writing " dir/command " in full path char array.
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, command);
        if (is_executable(full_path)) // and now using the access func we get know if the given file path is executable or not.
        {
            free(path_copy);
            return full_path;
        }
        dir = strtok(NULL, ":");
    }
    free(path_copy);
    return NULL;
}

void execute_cmd(char **argv, int options)
{ // 0-write to stdout, 1-write to my file.2-err
    pid_t pid = fork();
    if (pid == 0) // child process.
    {
        if (options == 1)
        {
            freopen("store_output.txt", "w", stdout);
        }
        if (options == 2)
        {
            freopen("store_output.txt", "w", stderr);
        }
        execvp(argv[0], argv);
        perror("execv");
        exit(1);
    }
    else if (pid < 0)
    {
        perror("fork");
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
    }
}

void redirect_output(char *str) // the whole command.
{
    char *dup = strdup(str);
    int option;
    if (!dup)
    {
        perror("strdup");
        return;
    }

    char *redirect_pos = NULL;
    // need to deal with 2> now this redirects the error output to specified file
    // successful output prints to terminal only.
    if (strstr(str, "1>"))
    {
        redirect_pos = strstr(dup, "1>"); // returns a pointer to the beginning of the substring
    }
    else if (strstr(str, "2>"))
    {
        redirect_pos = strstr(dup, "2>");
    }
    else
    {
        redirect_pos = strchr(dup, '>');
    }
    char *command = dup;
    char *filePath = NULL;

    if (redirect_pos[0] == '1')
    {
        *redirect_pos = '\0';
        filePath = redirect_pos + 2; // Skip "1>"
        option = 1;
    }
    else if (redirect_pos[0] == '2')
    {
        *redirect_pos = '\0';
        option = 2;
        filePath = redirect_pos + 2;
    }
    else
    {
        *redirect_pos = '\0';
        filePath = redirect_pos + 1; //> skipped
        option = 1;
    }

    if (filePath) /// ls >      test.txt
    {
        removeSpaces(filePath);
        if (strlen(filePath) == 0)
        {
            printf("syntax error: no file specified for redirection\n");
            free(dup);
            return;
        }
    }
    else
    {
        printf("syntax error: invalid redirection\n");
        free(dup);
        return;
    }

    char *cmd_copy = strdup(command); // full command like ls > test.txt
    if (!cmd_copy)
    {
        perror("strdup");
        free(dup);
        return;
    }

    char *argv[10];
    memset(argv, 0, sizeof(argv));
    int k = 0;
    char *token = strtok(cmd_copy, " ");
    while (token != NULL && k < 9)
    {
        argv[k++] = token;
        token = strtok(NULL, " ");
    }
    argv[k] = NULL;

    if (k == 0 || !argv[0])
    {
        printf("syntax error: command required\n");
        free(cmd_copy);
        free(dup);
        return;
    }

    char *path = find_in_path(argv[0]);
    if (path != NULL)
    {
        execute_cmd(argv, option); // 1->writing to store_output.txt
        // the command got executed and its output is written to the store_output.txt file
        // noow i need to read the file output file and write that to the redirect out put file.
        FILE *fptr_r = fopen("store_output.txt", "r");
        if (!fptr_r)
        {
            perror("fopen");
            free(cmd_copy);
            free(dup);
            return;
        }

        fseek(fptr_r, 0, SEEK_END);    // pointer to the end of file
        long fileSize = ftell(fptr_r); // offset from begin to curr pointer poosn
        rewind(fptr_r);                // pointer goes to the begin

        char *buff = (char *)malloc(fileSize + 1);

        if (!buff)
        {
            perror("malloc");
            fclose(fptr_r);
            free(cmd_copy);
            free(dup);
            return;
        }

        if (fread(buff, 1, fileSize, fptr_r) != fileSize)
        {
            perror("fread");
            free(buff);
            fclose(fptr_r);
            free(cmd_copy);
            free(dup);
            return;
        }
        buff[fileSize] = '\0';
        fclose(fptr_r);

        FILE *fptr_w = fopen(filePath, "w");
        if (!fptr_w)
        {
            perror("fopen");
            free(buff);

            free(cmd_copy);
            free(dup);
            return;
        }

        fwrite(buff, 1, strlen(buff), fptr_w);
        fclose(fptr_w);
        free(buff);
    }
    else
    {
        printf("%s: command not found\n", argv[0]);
    }

    free(cmd_copy);
    free(dup);
}