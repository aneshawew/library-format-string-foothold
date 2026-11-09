/* server.c
 *
 * Reproduced, unmodified, from SEED Labs' Format String Attack Lab
 * server-code/server.c. Copyright (c) 2018-2020 Wenliang Du.
 * Licensed CC BY-NC-SA 4.0 (free for non-commercial educational use).
 * See ATTRIBUTION.txt.
 *
 * A small forking TCP server: it accepts a connection, redirects the
 * connection's socket onto the child's stdin, and execs ./format (built
 * alongside this file) so the vulnerable program reads a remote client's
 * input, exactly as in the original lab.
 *
 * One change from the file as shipped in Labsetup.zip: the original has
 * an optional stdout-redirect block, commented out, with the inline note
 * "useful for remote attacks". Our topology is exactly that remote case
 * (attacker and server are separate hosts, unlike the stock lab where a
 * student runs both ends on their own machine via docker-compose and
 * reads the banner straight from `docker-compose logs`), so this sandbox
 * enables that block: the child connects back to the requesting client on
 * port 9091 and sends its stdout there. Clients must listen on 9091
 * before connecting to 9090 to see the banner. No other logic changed.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>

#define PROGRAM "format"
#define PORT    9090

int socket_bind(int port);
int server_accept(int listen_fd, struct sockaddr_in *client);
char **generate_random_env();

void main()
{
    int listen_fd;
    struct sockaddr_in  client;

    // Generate a random number
    srand (time(NULL));
    int random_n = rand()%2000;

    // handle signal from child processes
    signal(SIGCHLD, SIG_IGN);

    listen_fd = socket_bind(PORT);
    while (1){
	int socket_fd = server_accept(listen_fd, &client);

        if (socket_fd < 0) {
	    perror("Accept failed");
            exit(EXIT_FAILURE);
        }

	int pid = fork();
        if (pid == 0) {
            // Redirect STDIN to this connection, so it can take input from user
            dup2(socket_fd, STDIN_FILENO);

            // Connect back to the client on port 9091 and redirect STDOUT
            // there too - see the note above this function for why this
            // sandbox enables it (it's optional in the stock lab file).
            int output_fd = socket(AF_INET, SOCK_STREAM, 0);
            client.sin_port = htons(9091);
            if (!connect(output_fd, (struct sockaddr *)&client, sizeof(struct sockaddr_in))){
               dup2(output_fd, STDOUT_FILENO);
            }

	    // Invoke the program
	    fprintf(stderr, "Starting %s\n", PROGRAM);

	    // Using the following to pass a randomly generated environment variable array.
	    // This is useful to slightly randomize the stack's starting point.
            execle(PROGRAM, PROGRAM, (char *)NULL, generate_random_env(random_n));
        }
        else {
            close(socket_fd);
	}
    }

    close(listen_fd);
}


int socket_bind(int port)
{
    int listen_fd;
    int opt = 1;
    struct sockaddr_in server;

    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    memset((char *) &server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);

    if (bind(listen_fd, (struct sockaddr *) &server, sizeof(server)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(listen_fd, 3) < 0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    return listen_fd;
}

int server_accept(int listen_fd, struct sockaddr_in *client)
{
    int c = sizeof(struct sockaddr_in);

    int socket_fd = accept(listen_fd, (struct sockaddr *)client, (socklen_t *)&c);
    char *ipAddr = inet_ntoa(client->sin_addr);
    printf("Got a connection from %s\n", ipAddr);
    return socket_fd;
}

// Generate environment variables. The length of the environment affects
// the stack location. This is used to add some randomness to the lab.
char **generate_random_env(int length)
{
    const char *name = "randomstring=";
    char **env;

    env = malloc(2*sizeof(char *));

    env[0] = (char *) malloc((length + strlen(name))*sizeof(char));
    strcpy(env[0], name);
    memset(env[0] + strlen(name), 'A', length -1);
    env[0][length + strlen(name) - 1] = 0;
    env[1] = 0;
    return env;
}
