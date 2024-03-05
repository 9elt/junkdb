#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "config.hpp"
#include "router.hpp"

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("usage: %s <request>\n", argv[0]);
        return 1;
    }

    char *request = argv[1];

    char buffer[128]{0};

    int fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd == -1) {
        printf("cannot open socket");
        return 1;
    }

    sockaddr_in serv_addr;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(PORT);

    if (connect(fd, (sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("cannot connect to server");
        return 1;
    }

    send(fd, request, strlen(request), 0);

    while (recv(fd, buffer, sizeof(buffer), 0) > 0) {
        printf("%s\n", buffer);
        bzero(buffer, 3);
    }

    return 0;
}
