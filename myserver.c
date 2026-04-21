#include "util.h"
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
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }

    int listenfd = open_listenfd(argv[1]);
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

        close(connfd); // close connfd as soon as it's done servicing to its
                       // client
    }

    close(listenfd); // close listenfd when the server stops
    exit(EXIT_SUCCESS);
}
