#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_types/_socklen_t.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAXLINE 256

int main(int argc, char** argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }

    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo)); // initialize hints to 0
    hints.ai_family = PF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;

    struct addrinfo* res;
    int e;
    if ((e = getaddrinfo(NULL, argv[1], &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(e));
        exit(EXIT_FAILURE);
    }

    struct addrinfo* p;
    int listenfd;
    int optval = 1;
    for (p = res; p; p = p->ai_next) {
        if ((listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
            < 0) {
            continue;
        }
        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval,
            sizeof(optval));

        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0) {
            break;
        }
        close(listenfd);
    }

    if (!p) {
        // No address worked if p is NULL
        fprintf(stderr, "Error: No addresses worked.\n");
        exit(EXIT_FAILURE);
    }

    char hostname[MAXLINE];
    char port[MAXLINE];
    getnameinfo(p->ai_addr, p->ai_addrlen, hostname, MAXLINE, port, MAXLINE, 0);
    printf("A server created at (%s, %s)\n", hostname, port);

    freeaddrinfo(res);

    if (listen(listenfd, 10) < 0) {
        fprintf(stderr, "listen error\n");
        exit(EXIT_FAILURE);
    }
    printf("Waiting for a connection...\n\n");

    struct sockaddr address;
    socklen_t address_len;
    int connfd;

    char buf[MAXLINE];
    int rc;

    while (1) {
        connfd = accept(listenfd, &address, &address_len);
        printf("Connected!\n\n");
        rc = read(connfd, (void*)buf, MAXLINE);
        printf("Read %d bytes:\n", rc);
        printf("%s\n\n", buf);

        close(connfd);
    }

    // at this point, a connection must have been established

    close(listenfd);
    exit(EXIT_SUCCESS);
}
