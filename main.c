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
    char *path_env = getenv("PATH");
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

void execute_cmd(char *path, int argc, char **argv, int options)
{ // 0-write to stdout, 1-write to my file.2-err
    pid_t pid = fork();
    if (pid == 0) // child process.
    {
        if (options == 1)
        {
            freopen("store_output.txt", "w", stdout); // replaces the stdout stream with the file
                                                      // so everything written inside my file for this child process.
        }
        else if (options == 2)
        {
            freopen("store_output.txt", "w", stderr);
        }
        execv(path, argv);
        perror("execv");
        exit(1);
    }
    else if (pid < 0)
        perror("fork");
    else // parent waiting for the child
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
        filePath = redirect_pos + 1;
        option = 1;
    }

    if (filePath)
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

    char *cmd_copy = strdup(command);
    if (!cmd_copy)
    {
        perror("strdup");
        free(dup);
        return;
    }

    char *argv[10];
    memset(argv, 0, sizeof(argv));
    int k = 0;

    char *token = strtok(cmd_copy, " \t");
    while (token != NULL && k < 9)
    {
        argv[k++] = token;
        token = strtok(NULL, " \t");
    }
    argv[k] = NULL;

    if (k == 0 || !argv[0])
    {
        printf("syntax error: command required\n");
        free(cmd_copy);
        free(dup);
        return;
    }

    if (strcmp(argv[0], "pwd") == 0)
    {

        char *buff = (char *)malloc(sizeof(char) * 4096);
        if (!buff)
        {
            perror("malloc");
            free(cmd_copy);
            free(dup);
            return;
        }

        if (getcwd(buff, 4096) != NULL)
        {
            FILE *fptr_w = fopen(filePath, "w");
            if (!fptr_w)
            {
                perror("fopen");
                free(buff);
                free(cmd_copy);
                free(dup);
                return;
            }
            fprintf(fptr_w, "%s\n", buff);
            fclose(fptr_w);
        }
        else
        {
            perror("pwd");
        }
        free(buff);
    }
    else
    {
        char *path = find_in_path(argv[0]);
        if (path != NULL)
        {

            execute_cmd(path, k, argv, option); // 1->writing to store_output.txt
            FILE *fptr_r = fopen("store_output.txt", "r");
            if (!fptr_r)
            {
                perror("fopen");
                free(cmd_copy);
                free(dup);
                return;
            }

            fseek(fptr_r, 0, SEEK_END);
            long fileSize = ftell(fptr_r);
            rewind(fptr_r);

            char *buff = (char *)malloc(fileSize + 1);
            char *temp_buff = (char *)malloc(fileSize + 1);
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
            int ptr = 0;
            for (int j = 0; j < fileSize; j++)
            {
                if (buff[j] != '\'')
                {
                    temp_buff[ptr++] = buff[j];
                }
            }
            temp_buff[ptr] = '\0';

            FILE *fptr_w = fopen(filePath, "w");
            if (!fptr_w)
            {
                perror("fopen");
                free(buff);
                free(temp_buff);
                free(cmd_copy);
                free(dup);
                return;
            }

            fwrite(temp_buff, 1, strlen(temp_buff), fptr_w);
            fclose(fptr_w);
            free(buff);
            free(temp_buff);
        }
        else
        {
            printf("%s: command not found\n", argv[0]);
        }
    }

    free(cmd_copy);
    free(dup);
}
void removeSpaces(char *str)
{
    int i = 0;

    // Find the first non-space character
    while (str[i] && isspace(str[i]))
    {
        i++;
    }

    // Shift the rest of the string to the beginning
    int j = 0;
    while (str[i])
    {
        str[j++] = str[i++];
    }

    str[j] = '\0'; // Null-terminate the new string
}
void process_input(char *input, char *processed_input)
{
    int i = 0;
    int j = 0;
    int in_quotes = 0;
    char quote_char = 0;

    while (input[i])
    {

        if ((input[i] == '\'' || input[i] == '"') && !in_quotes)
        {
            in_quotes = 1;
            quote_char = input[i];
            i++;
            continue;
        }
        else if (in_quotes && input[i] == quote_char)
        {
            in_quotes = 0;
            i++;
            continue;
        }

        processed_input[j++] = input[i++];
    }
    processed_input[j] = '\0';
}
int main()
{
    // Flush after every printf
    setbuf(stdout, NULL); // so jha bhi printf hoga rather than storing the value to a buffer it will be flushed directly to stdout.

    while (1)
    {
        printf("$ ");

        // Wait for user input
        char input[100]; // this memeory allocated on stack
        fgets(input, 100, stdin);

        input[strcspn(input, "\n")] = 0; // strcspn-->return the index of the specified char in the buffer.
        // fgets()->automatically adds  a \n character to the end of the input we remove it.

        // i think to implement the redirect feature i need to look for the redirect operator
        // and then split the string based on that and get the src command and destination file
        char clean_input[100];
        process_input(input, clean_input);
        if (strchr(clean_input, '>') != NULL)
        {
            redirect_output(clean_input);
            continue; // don't need to do anything else after redirecting the output.
        }

        if (strcmp(clean_input, "exit 0") == 0)
        {
            break;
        }
        else if (strcmp(clean_input, "") == 0)
        {
            continue;
        }
        char *inp_dup = strdup(clean_input);
        char *token = strtok(inp_dup, " "); // returns the first word. before space

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
                // Search for the command in the PATH enviornment variable,
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
        else if (strcmp(token, "cd") == 0)
        {
            char *abs_dir = strtok(NULL, " ");
            if (abs_dir == NULL)
            {
                continue;
            }
            if (strcmp(abs_dir, "~") == 0)
            { // change to home director/
                // has the path to home directory..
                abs_dir = getenv("HOME");
            }

            if (chdir(abs_dir) == -1)
            {
                printf("cd: %s: No such file or directory\n", abs_dir);
                continue;
            }
        }
        else
        {
            // the first step would be to fork and create a child process that executes the
            // executable file of the command  typed in.
            // need to run this executable file to do that we use the execvp command
            char *argv[10]; // array of char pointers.

            memset(argv, 0, sizeof(argv));
            int k = 0;
            char *inp = strdup(clean_input);
            // printf("%s\n", inp_dup);
            char *cmd = strtok(inp, " ");
            while (cmd != NULL && k < 10)
            {
                argv[k++] = cmd;
                cmd = strtok(NULL, " ");
            }
            argv[k] = NULL;
            if (strcmp(argv[0], "pwd") == 0)
            {
                char *buff = (char *)malloc(sizeof(char) * 4096);
                if (buff == NULL)
                {
                    perror("malloc");
                    free(inp);
                    continue;
                }
                if (getcwd(buff, 4096) != NULL)
                {
                    printf("%s\n", buff);
                }
                else
                {
                    perror("pwd");
                }
                free(buff);
            }
            // Handle external commands
            else
            {
                char *path = find_in_path(argv[0]);
                if (path != NULL)
                {
                    execute_cmd(path, k, argv, 0);
                }
                else
                {
                    printf("%s: command not found\n", argv[0]);
                }
            }
            free(inp);
        }
    }
    return 0;
}
