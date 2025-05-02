#include <stdio.h>
#include <string.h>

#define MAX_STRING_SIZE 100

void flush_input(char userInput[]);

int main(void)
{
    char input[MAX_STRING_SIZE] = {0};
    int i;
    int upArrow = 0; // 1 if up arrow found, 0 if not found

    printf("Enter anything with an up arrow key (or not if you so choose):\n");
    fgets(input, sizeof(input), stdin);
    flush_input(input);

    size_t inputLen = strlen(input);

    for (i = 0; i < (int)inputLen; i++)
    {
        if (input[i] == '\33')
        {
            if (input[i + 2] == 'A')
            {
                upArrow = 1;
            }
        }
    }

    if (upArrow != 0)
    {
        printf("You entered an up arrow somewhere in the input\n");
    }
    else
    {
        printf("You did not enter an up arrow anywhere\n");
    }
    return 0;
}

void flush_input(char userInput[])
{
    if (strchr(userInput, '\n') == NULL)
    {
        /* If there is no new line found in the input, it means the [enter] character
        was not read into the input, so fgets() must have truncated the input
        therefore it would not put the enter key into the input.  The user would
        have typed in too much, so we would have to clear the buffer. */
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF)
            ;
    }
    else
    {
        // If there is a new line, lets remove it from our input.  fgets() did not need to truncate
        userInput[strcspn(userInput, "\r\n")] = 0;
    }
}