#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <ctype.h>

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