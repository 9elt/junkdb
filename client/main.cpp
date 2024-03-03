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
        printf("usage: %s <request> options\n", argv[0]);
        return 1;
    }

    char *request = argv[1];

    bool raw = true;

    if (argc > 2) {
        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--human") == 0) {
                raw = false;
            }
        }
    }

    char buffer[3]{0};

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

        if (raw) {
            printf("%s\n", buffer);
        } else if (strcmp(buffer, OK) == 0) {
            printf("ok\n");
        } else if (strcmp(buffer, TRUE) == 0) {
            printf("true\n");
        } else if (strcmp(buffer, FALSE) == 0) {
            printf("false\n");
        } else if (strcmp(buffer, UNKNOWN_ACTION) == 0) {
            printf("unknown action\n");
        } else if (strcmp(buffer, UNREACHABLE_DATABASE) == 0) {
            printf("unreachable database\n");
        } else if (strcmp(buffer, SERVER_ERROR) == 0) {
            printf("server error\n");
        } else {
            printf("unknown response: %s\n", buffer);
        }

        bzero(buffer, 3);
    }

    return 0;
}
