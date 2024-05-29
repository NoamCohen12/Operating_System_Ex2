// the command ./mync -e "./ttt 123456789"
#include <sys/types.h>
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
#include <netdb.h>
#include <signal.h>
#define SOCKET_PATH "/tmp/unix_socket"

// serverport-  the port users will be connecting to
int numbytes = 101;

int open_client_unix_datagram()
{
    int sockfd;
    struct sockaddr_un addr;

    // Create a Unix domain socket
    sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Initialize address structure
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1)
    {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

int open_client_unix_stream()
{
    int sockfd;
    struct sockaddr_un addr;

    // Create a Unix domain socket
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Initialize address structure
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1)
    {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

int open_server_unix_stream()
{
    int sockfd;
    struct sockaddr_un addr;

    // Create a Unix domain socket
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Ensure the socket file does not already exist
    unlink(SOCKET_PATH);

    // Initialize address structure
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    // Bind the socket to the address
    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1)
    {
        perror("bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(sockfd, 5) == -1)
    {
        perror("listen");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

int open_server_unix_udp()
{
    int sockfd;
    struct sockaddr_un addr;

    // Create a Unix domain socket
    sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Ensure the socket file does not already exist
    unlink(SOCKET_PATH);

    // Initialize address structure
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    // Bind the socket to the address
    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1)
    {
        perror("bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    return sockfd;
}
int open_client_UDP(char *destaddr, char *serverport)
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // set to AF_INET to use IPv4
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(destaddr, serverport, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1)
        {
            perror("talker: socket");
            continue;
        }

        break;
    }
    connect(sockfd, p->ai_addr, p->ai_addrlen);
    return sockfd;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int open_server_UDP(char *myport)
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    char buf[numbytes];
    socklen_t addr_len;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // set to AF_INET to use IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, myport, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1)
        {
            perror("listener: socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("listener: bind");
            continue;
        }
        break;
    }
    if (p == NULL)
    {
        fprintf(stderr, "listener: failed to bind socket\n");
        return 2;
    }
    freeaddrinfo(servinfo);
    return sockfd;
}

/*
open socket - server side
return the client fd
*/
int open_server_TCP(char *port)
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
    // printf("socket(2) Sockfd = %d\n", sockfd);
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
    if ((client_fd = accept(sockfd, (struct sockaddr *)&clientadder, &addrlen)) == -1)
    {
        perror("accept");
        exit(1);
    }
    // printf("accept(2) Client_fd = %d\n", client_fd);
    return client_fd;
}

/**
open socket - client side
return the ----------------------------------
*/
int open_client_TCP(char *server_ip, char *server_port)
{
    struct addrinfo hints, *res, *p;
    int status;
    int sockfd;

    // set up the hints structure
    memset(&hints, 0, sizeof hints);
    hints.ai_socktype = SOCK_STREAM;

    // get address info
    if ((status = getaddrinfo(server_ip, server_port, &hints, &res)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    }

    // loop through the results and connect to the first we can
    for (p = res; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("error creating socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("error connecting to server");
            continue;
        }

        break; // if we get here, we must have connected successfully
    }

    if (p == NULL)
    {
        perror("failed to connect\n");
    }

    freeaddrinfo(res); // free the linked list
    return sockfd;
}

int run_programming(char *input)
{
    // Ensure input is not NULL
    if (input == NULL)
    {
        perror("Input string is NULL\n");
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
    // for (int i = 0; i < argc; i++)
    // {
    //     printf("argv[%d]: %s\n", i, argv[i]);
    // }
    // Check if any command was provided
    if (argc == 0)
    {
        perror("Without arguments\n");
        return 1;
    }
    // printf("before fork\n");
    //  Fork and execute the command
    int pid;
    if ((pid = fork()) == -1)
    {
        perror("fork failed");
        return 1;
    }
    if (pid == 0)
    {
        // the programinig run untill here
        //  Child process: execute the command
        execvp(argv[0], argv);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }
    // Parent process: wait for the child to complete
    wait(NULL);
    fflush(stdout);
    printf("End of run_programming\n");

    return 0;
}

int i_case(char *input)
{
    if (strncmp(input, "UDPS", 4) == 0)
    {
        char *port = input + 4; // port start after 4 chars
        // printf("port: %s\n", port);
        int c_fd = open_server_UDP(port);
        // printf("c_fd in i case: %d\n", c_fd);
        if (dup2(c_fd, STDIN_FILENO) == -1)
        {
            perror("dup2- UDPS i case");
            close(c_fd);
        }
    }

    // printf("input out: %s\n", input);
    if (strncmp(input, "TCPS", 4) == 0)
    {
        char *port = input + 4; // port start after 4 chars
        // printf("port: %s\n", port);
        int c_fd = open_server_TCP(port);
        // printf("c_fd in i case: %d\n", c_fd);
        if (dup2(c_fd, STDIN_FILENO) == -1)
        {
            perror("dup2- TCPS i case");
            close(c_fd);
        }
    }
    if (strncmp(input, "TCPC", 4) == 0)
    {
        input = input + 4; // port start after 4 chars
        char *localhost;
        if ((localhost = strtok(input, ",")) == NULL)
        {
            perror("Didn't get localhost");
            exit(1);
        }
        char *port_char;
        if ((port_char = strtok(NULL, ",")) == NULL)
        {
            perror("Didn't get port");
            exit(1);
        }
        int sockfd = open_client_TCP(localhost, port_char);
        if (dup2(sockfd, STDIN_FILENO) == -1)
        {
            perror("dup2- TCPC i case");
            close(sockfd);
        }
    }
     if (strncmp(o_value, "UDSS", 4) == 0)
    {
        input = input + 4; // check the input after 4 chars D or S
        if (strncmp(input, "D", 1) == 0)
        {
            input++;
            int sockfd = open_server_unix_udp();
        }
        else if (strncmp(input, "S", 1) == 0)
        {
            input++;
            int sockfd = open_server_unix_stream();
        }
    return 0;
}
int o_case(char *input)
{

    // printf("input out: %s\n", input);
    if (strncmp(input, "TCPS", 4) == 0)
    {
        char *port = input + 4; // port start after 4 chars
        // printf("port o case: %s\n", port);
        int c_fd = open_server_TCP(port);
        // printf("c_fd in o case: %d\n", c_fd);
        if (dup2(c_fd, STDOUT_FILENO) == -1)
        {
            perror("dup2- TCPS o case");
            close(c_fd);
        }
    }
    if (strncmp(input, "TCPC", 4) == 0)
    {
        input = input + 4; // port start after 4 chars
        char *localhost;
        if ((localhost = strtok(input, ",")) == NULL)
        {
            perror("no localhost");
            exit(1);
        }
        char *port_char;
        if ((port_char = strtok(NULL, ",")) == NULL)
        {
            perror("no port");
            exit(1);
        }
        int sockfd = open_client_TCP(localhost, port_char);
        if (dup2(sockfd, STDOUT_FILENO) == -1)
        {
            perror("dup2- TCPC o case");
            close(sockfd);
        }
    }
    if (strncmp(input, "UDPC", 4) == 0)
    {
        input = input + 4; // port start after 4 chars
        char *localhost;
        if ((localhost = strtok(input, ",")) == NULL)
        {
            perror("no localhost");
            exit(1);
        }
        char *port_char;
        if ((port_char = strtok(NULL, ",")) == NULL)
        {
            perror("no port");
            exit(1);
        }
        int sockfd = open_client_UDP(localhost, port_char);
        if (dup2(sockfd, STDOUT_FILENO) == -1)
        {
            perror("dup2- UDP o case");
            close(sockfd);
        }
    }
    if (strncmp(o_value, "UDSC", 4) == 0)
    {
        input = input + 4; // check the input after 4 chars D or S
        if (strncmp(input, "D", 1) == 0)
        {
            input++;
            int sockfd = open_client_unix_datagram();
        }
        else if (strncmp(input, "S", 1) == 0)
        {
            input++;
            int sockfd = open_client_unix_stream();
        }

        return 0;
    }
    // both- i and o
    int b_case(char *input)
    {
        if (strncmp(input, "TCPS", 4) == 0)
        {
            char *port = input + 4; // port start after 4 chars
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
            char *localhost;
            if ((localhost = strtok(input, ",")) == NULL)
            {
                perror("Didn't get localhost");
                exit(1);
            }
            char *port_char;
            if ((port_char = strtok(NULL, ",")) == NULL)
            {
                perror("Didn't get port");
                exit(1);
            }

            int sockfd = open_client_TCP(localhost, port_char);
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

    void handle_alarm(int sig)
    {
        printf("Time out\n");
        exit(1);
    }

    int t_case(char *time_in_sec)
    {
        printf("Time: %s\n", time_in_sec);
        if (time_in_sec == NULL)
        {
            perror("Time is NULL\n");
            exit(1);
        }
        int time = atoi(time_in_sec);
        if (time < 0)
        {
            perror("Time should be positive\n");
            exit(1);
        }
        signal(SIGALRM, handle_alarm);
        alarm(time);
        return 0;
    }

    int main(int argc, char *argv[])
    {
        if (argc < 2)
        {
            perror("Error\n");
            exit(1);
        }
        char *re_val_e;
        int re_val_b;
        int re_val_i;
        int re_val_o;
        int re_val_timeout;
        // optarg
        int opt;
        while ((opt = getopt(argc, argv, "e:b:i:o:t:")) != -1)
        {
            {
                switch (opt)
                {
                case 'e':
                    re_val_e = optarg; // optarg is the argument after -e
                    break;
                case 'b':
                    re_val_b = b_case(optarg); // optarg is the argument after -b
                    break;
                case 'i':
                    re_val_i = i_case(optarg); // optarg is the argument after -i
                    break;
                case 'o':
                    re_val_o = o_case(optarg); // optarg is the argument after -i
                    break;
                case 't':
                    re_val_timeout = t_case(optarg); // optarg is the argument after -i
                    break;

                default:
                    fprintf(stderr, "You should write Usage: %s -e <value>\n", argv[0]);
                    exit(EXIT_FAILURE);
                }
            }
        }
        run_programming(re_val_e);
        printf("end");
        close(1);
    }
