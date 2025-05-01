#include <stdio.h>
#include <string.h>
#include <stdlib.h>
char *process_input(char *input) // removes the quotes and slashes.
{
    char *processed_input = (char *)malloc(strlen(input) * sizeof(char));
    int i = 0;
    int j = 0;
    int in_quotes = 0;
    char quote_char = 0;

    while (input[i])
    {
        if ((input[i] == '\'' || input[i] == '"') && !in_quotes) // quotes start.
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
    return processed_input;
}

char **parse_command(char *input)
{
    char *dup = strdup(input);
    char **arr = (char **)malloc(10 * sizeof(char *));
    char *token = strtok(dup, " ");
    arr[0] = token;
    int j = 1;
    while (token != NULL)
    {
        token = strtok(NULL, " ");
        arr[j++] = token;
    }
    arr[j] = NULL;
    return arr;
}