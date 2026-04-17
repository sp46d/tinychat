#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAXLINE 256

int main(int argc, char** argv)
{
    if (argc != 3) {
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
        exit(0);
    }

    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = PF_INET;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo* res;
    int e;
    if ((e = getaddrinfo(argv[1], argv[2], &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(e));
        exit(EXIT_FAILURE);
    }

    struct addrinfo* p;
    int clientfd;
    for (p = res; p; p = p->ai_next) {
        if ((clientfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
            < 0) {
            continue;
        }
        // try to connect to a server
        if (connect(clientfd, p->ai_addr, p->ai_addrlen) == 0) {
            // a connection succeeds!
            break;
        }
        close(clientfd);
    }

    if (!p) {
        // no connections succeeded if p is NULL
        fprintf(stderr, "Connection failed\n");
        exit(EXIT_FAILURE);
    }

    char host[MAXLINE];
    char port[MAXLINE];
    getnameinfo(p->ai_addr, p->ai_addrlen, host, MAXLINE, port, MAXLINE, 0);

    printf("Connected to the server at (%s, %s)\n", host, port);

    char buf[MAXLINE];
    while (fgets(buf, MAXLINE, stdin) != NULL) {
        write(clientfd, (void*)buf, strlen(buf));
    }

    close(clientfd);
    exit(EXIT_SUCCESS);
}
