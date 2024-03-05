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

    bool strip_response_status = false;

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--strip") == 0 || strcmp(argv[i], "-s") == 0) {
            strip_response_status = true;
        }
    }

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
        if (strip_response_status) {
            int offset = buffer[0] == 'O' ? 3 : 4; // 'OK ' or 'ERR

            if (buffer[offset - 1] != '\0') {
                printf("%s", buffer + offset);
            }
        } else {
            printf("%s\n", buffer);
        }
        bzero(buffer, 3);
    }

    return 0;
}
