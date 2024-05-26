// mync -e "ttt 123456789"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
int run_programming(char *input)
{
    // Ensure input is not NULL
    if (input == NULL)
    {
        fprintf(stderr, "Input string is NULL\n");
        return 1;
    }

    // Duplicate the input string to avoid modifying the original
    char *args_as_string = strdup(input);
    if (args_as_string == NULL)
    {
        perror("strdup");
        return 1;
    }

    // Array to hold the command and its arguments (assuming max 20 arguments)
    char *argv[20];
    int argc = 0;

    // Tokenize the input string
    char *token = strtok(args_as_string, " ");
    while (token != NULL && argc < 19)
    {
        argv[argc++] = token;
        token = strtok(NULL, " ");
    }
    argv[argc] = NULL; // Null-terminate the array

    // Check if any command was provided
    if (argc == 0)
    {
        fprintf(stderr, "Without arguments\n");
        free(args_as_string);
        return 1;
    }

    // Execute the command
    execvp(argv[0], argv);

    // If execvp returns, there was an error
    perror("execvp failed");
    free(args_as_string);
    return 1;
}










int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Error\n");
        exit(1);
    }
    // optarg
    int opt;
    while ((opt = getopt(argc, argv, "e:")) != -1)
    {
        switch (opt)
        {
        case 'e':
        run_programming(optarg);
            break;
        default:
            fprintf(stderr, "You should write Usage: %s -e <value>\n", argv[0]);

            exit(EXIT_FAILURE);
        }
    }
}