#include "util.h"
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAXLINE 256

int open_clientfd(char* host, char* port);

int main(int argc, char** argv)
{
    if (argc != 3) {
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
        exit(0);
    }

    int clientfd;
    char buf[MAXLINE];
    while (1) {
        clientfd = open_clientfd(argv[1], argv[2]);
        fgets(buf, MAXLINE, stdin);
        write(clientfd, (void*)buf, strlen(buf));
    }

    close(clientfd);
    exit(EXIT_SUCCESS);
}
