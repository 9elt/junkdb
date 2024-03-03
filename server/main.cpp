#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "router.hpp"

int main() {
    Active databases;
    Router router(&databases);

    int fd;
    int sfd;
    int opt = 1;

    char buffer[4096];

    sockaddr_in serv_addr;
    sockaddr_in cli_addr;

    socklen_t socklen = sizeof(serv_addr);

    fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd == -1) {
        printf("socket creation failed");
        return 1;
    }

    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
        printf("setsockopt failed");
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(PORT);

    if (bind(fd, (sockaddr *)&serv_addr, socklen) < 0) {
        printf("bind failed");
        return 1;
    }

    printf("Server started at port %d\n", PORT);

    if (listen(fd, 32) == -1)
        close(fd);

    while (true) {
        sfd = accept(fd, (sockaddr *)&cli_addr, &socklen);

        if (sfd == -1) {
            printf("accept failed");
            continue;
        }

        bzero(buffer, 4096);

        if (read(sfd, buffer, 4095) < 0) {
            printf("read failed");
            close(sfd);
            continue;
        }

        printf("received: %s\n", buffer);

        Request request(buffer);

        char *response = (char *)router.handle(&request);

        send(sfd, response, strlen(response), 0);

        close(sfd);
    }

    return 0;
}
