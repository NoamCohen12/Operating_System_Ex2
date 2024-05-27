//the command ./mync -e "./ttt 123456789"
#include <sys/types.h> // Add this line
#include <errno.h>
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
int run_programming(char *input) {
    // Ensure input is not NULL
    if (input == NULL) {
        fprintf(stderr, "Input string is NULL\n");
        return 1;
    }

    // Array to hold the command and its arguments (assuming max 20 arguments)
    char *argv[20];
    int argc = 0;

    // Tokenize the input string
    char *token = strtok(input, " ");
    int i = 0;
    while (token != NULL && argc < 19) {
        printf("%d",i);
        argv[argc++] = token;
        token = strtok(NULL, " ");
        i++;
    }
    argv[argc] = NULL; // Null-terminate the array

    // Debug: Print each token to see the result of strtok
    for (int i = 0; i < argc; i++) {
        printf("argv[%d]: %s\n", i, argv[i]);
    }

    // Check if any command was provided
    if (argc == 0) {
        fprintf(stderr, "Without arguments\n");
        return 1;
    }

    // Fork and execute the command
    int pid;
    if ((pid = fork()) == -1) {
        perror("fork failed");
        return 1;
    } else if (pid == 0) {
        // Child process: execute the command
        execvp(argv[0], argv);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }
    // Parent process: wait for the child to complete
    wait(NULL);

    return 0;
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
            run_programming(optarg);//optarg is the argument after -e
            break;
        default:
            fprintf(stderr, "You should write Usage: %s -e <value>\n", argv[0]);

            exit(EXIT_FAILURE);
        }
    }
}