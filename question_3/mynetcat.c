// the command ./mync -e "./ttt 123456789"
#include <sys/types.h> // Add this line
#include <errno.h>
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

/*
open socket - server side
return the client fd
*/
int open_server_TCP(int port)
{
    struct sockaddr_in serveradder;
    struct sockaddr_in clientadder;
    socklen_t addrlen;
    // int buflen = 100;
    // char buf[buflen];
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    serveradder.sin_family = AF_INET;
    serveradder.sin_port = htons(atoi(port));
    serveradder.sin_addr.s_addr = INADDR_ANY;
    printf("socket(2) Sockfd = %d\n", sockfd);
    if (bind(sockfd, (struct sockaddr *)&serveradder, sizeof(serveradder)) == -1)
    {
        perror("bind");
        exit(1);
    }
    if (listen(sockfd, 5) == -1)
    {
        perror("listen");
        exit(1);
    }
    int client_fd;
    if (client_fd = accept(sockfd, (struct sockaddr *)&clientadder, &addrlen) == -1)
    {
        perror("accept");
        exit(1);
    }
    return client_fd;
}

/**
open socket - client side
return the ----------------------------------
*/
int open_client_TCP(char *ip, char *port)
{
    struct sockaddr_in serveradder;
    socklen_t socklen = sizeof(serveradder);
    int sockfd;
    if (sockfd = socket(AF_INET, SOCK_STREAM, 0) == -1)
    {
        perror("socket");
        exit(1);
    }
    serveradder.sin_family = AF_INET;
    serveradder.sin_port = htons(htoi(port));
    serveradder.sin_addr.s_addr = inet_addr(ip);
    if (connect(sockfd, (struct sockaddr *)&serveradder, socklen) == -1)
    {
        perror("connect");
        exit(1);
    }
    close(1);
    dup(sockfd);
    printf("hello world\n");
    exit(0);
    return sockfd;
}

int run_programming(char *input)
{
    // Ensure input is not NULL
    if (input == NULL)
    {
        fprintf(stderr, "Input string is NULL\n");
        return 1;
    }

    // Array to hold the command and its arguments (assuming max 20 arguments)
    char *argv[20];
    int argc = 0;

    // Tokenize the input string
    char *token = strtok(input, " ");
    int i = 0;
    while (token != NULL && argc < 19)
    {
        printf("%d", i);
        argv[argc++] = token;
        token = strtok(NULL, " ");
        i++;
    }
    argv[argc] = NULL; // Null-terminate the array

    // Debug: Print each token to see the result of strtok
    for (int i = 0; i < argc; i++)
    {
        printf("argv[%d]: %s\n", i, argv[i]);
    }

    // Check if any command was provided
    if (argc == 0)
    {
        fprintf(stderr, "Without arguments\n");
        return 1;
    }

    // Fork and execute the command
    int pid;
    if ((pid = fork()) == -1)
    {
        perror("fork failed");
        return 1;
    }
    else if (pid == 0)
    {
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
    int re_val_e;
    int re_val_b;
    int re_val_i;
    int re_val_o;
    // optarg
    int opt;
    while ((opt = getopt(argc, argv, "e:b:i:o:")) != -1)
    {
        {
            switch (opt)
            {
            case 'e':
                re_val_e = e_case(optarg); // optarg is the argument after -e
                break;
            case 'b':
                re_val_b = b_case(optarg); // optarg is the argument after -b
                break;
            case 'i':
                re_val_i = i_case(optarg); // optarg is the argument after -i
                break;
            case 'o':
                break;
                re_val_o = o_case(optarg); // optarg is the argument after -i
            default:
                fprintf(stderr, "You should write Usage: %s -e <value>\n", argv[0]);

                exit(EXIT_FAILURE);
            }
        }
    }
}

int i_case(char *input)
{
    if (strncmp(input, "TCPS", 4) == 0)
    {
        input = input + 4; // port start after 4 chars
        int port = atoi(input);
        // return client fd
        int c_fd = open_server_TCP(port);
        if (dup2(c_fd, STDIN_FILENO) == -1)
        {
            perror("dup2- TCPS i case");
            close(c_fd);
        }
    }
    if (strncmp(input, "TCPC", 4) == 0)
    {
        input = input + 4; // port start after 4 chars
        char localhost;
        if (localhost = strtok(input, ",") == NULL)
        {
            printf("Didn't get localhost");
            exit(1);
        }
        char port_char;
        if ( port_char = strtok(NULL, ",") == NULL)
        {
            printf("Didn't get port");
            exit(1);
        }
        int port = atoi(port_char);
        int sockfd = open_client_TCP(localhost, port);
        if (dup2(sockfd, STDIN_FILENO) == -1)
        {
            perror("dup2- TCPC i case");
            close(sockfd);
        }
    }
    return 0;
}
int o_case(char *input)
{
    char port = input + 4;
    if (strncmp(input, "TCPS", 4) == 0)
    {
        input = input + 4; // port start after 4 chars
        int port = atoi(input);
        int c_fd = open_server_TCP(port);
        if (dup2(c_fd, STDOUT_FILENO) == -1)
        {
            perror("dup2- TCPS o case");
            close(c_fd);
        }
    }
    if (strncmp(input, "TCPC", 4) == 0)
    {
        input = input + 4; // port start after 4 chars
        if (char localhost = strtok(input, ",") == NULL)
        {
            printf("no localhost");
            exit(1);
        }
        if (char port_char = strtok(NULL, ",") == NULL)
        {
            printf("no port");
            exit(1);
        }
        int port = atoi(port_char);
        int sockfd = open_client_TCP(localhost, port);
        if (dup2(sockfd, STDOUT_FILENO) == -1)
        {
            perror("dup2- TCPC o case");
            close(sockfd);
        }
    }
    return 0;
}
// both- i and o
int b_case(char *input)
{
    char port = input + 4;
    if (strncmp(input, "TCPS", 4) == 0)
    {
        input = input + 4; // port start after 4 chars
        int port = atoi(input);
        int c_fd = open_server_TCP(port);
        if (dup2(c_fd, STDOUT_FILENO) == -1)
        {
            perror("dup2- b TCPS o case");
            close(c_fd);
        }
        if (dup2(c_fd, STDIN_FILENO) == -1)
        {
            perror("dup2- b TCPS i case");
            close(c_fd);
        }
    }
    if (strncmp(input, "TCPC", 4) == 0)
    {
        input = input + 4; // port start after 4 chars
        if (char localhost = strtok(input, ",") == NULL)
        {
            printf("Didn't get localhost");
            exit(1);
        }
        if (char port_char = strtok(NULL, ",") == NULL)
        {
            printf("Didn't get port");
            exit(1);
        }
        int port = atoi(port_char);
        int sockfd = open_client_TCP(localhost, port);
        if (dup2(sockfd, STDOUT_FILENO) == -1)
        {
            perror("dup2- b TCPC o case");
            close(sockfd);
        }
        if (dup2(sockfd, STDIN_FILENO) == -1)
        {
            perror("dup2- b TCPC i case");
            close(sockfd);
        }
    }
    return 0;
}
int e_case(char *input)
{
   run_programming(input);
    return 0;
}