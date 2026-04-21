#include "util.h"
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int open_clientfd(char* host, char* port)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = PF_INET;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo* res;
    int e;
    if ((e = getaddrinfo(host, port, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(e));
        exit(-1);
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
        exit(-1);
    }
    freeaddrinfo(res);
    return clientfd;
}

int open_listenfd(char* port)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo)); // initialize hints to 0
    hints.ai_family = PF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;

    struct addrinfo* res;
    int e;
    if ((e = getaddrinfo(NULL, port, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(e));
        exit(-1);
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
        exit(-1);
    }
    freeaddrinfo(res);
    if (listen(listenfd, 10) < 0) {
        fprintf(stderr, "listen error\n");
        exit(-1);
    }
    return listenfd;
}
