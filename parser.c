#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

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

char **parse_command(char *input) // returns the arr of arguments for the command.
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