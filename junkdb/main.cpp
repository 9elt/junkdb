#include "router.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

const in_addr_t LOCALHOST = inet_addr("127.0.0.1");

const char *VERSION = "0.1.0";

const char *HELP = "usage: %s [options]\n"
                   "--help,    -h    show this help message\n"
                   "--version, -V    show version\n";

int main(int argc, char **argv) {
    const char *program = argv[0];

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            LOG_F(HELP, program);
            return 0;
        } else if (strcmp(argv[i], "--version") == 0 ||
                   strcmp(argv[i], "-V") == 0) {
            LOG_LF("junkdb version %s", VERSION);
            return 0;
        } else {
            FAIL_LF("Unknown option '%s'", argv[i]);
        }
    }

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
        FAIL_L("Failed to create socket");
    }

    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
        FAIL_L("Setsockopt failed");
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = LOCALHOST;
    serv_addr.sin_port = htons(PORT);

    if (bind(fd, (sockaddr *)&serv_addr, socklen) < 0) {
        FAIL_L("Socket bind failed");
    }

    LOG_LF("Server started at port %d", PORT);

    if (listen(fd, 32) == -1) {
        close(fd);
        FAIL_L("Socket listen failed");
    }

    while (true) {
        sfd = accept(fd, (sockaddr *)&cli_addr, &socklen);

        if (sfd == -1) {
            ERROR_L("Failed to accept request");
            continue;
        }

        if (cli_addr.sin_addr.s_addr != LOCALHOST) {
            ERROR_LF("Request from foreign hosts are not allowed, closing "
                     "connection with %s",
                     inet_ntoa(cli_addr.sin_addr));
            close(sfd);
            continue;
        }

        bzero(buffer, 4096);

        if (read(sfd, buffer, 4095) < 0) {
            ERROR_L("Failed to read request");
            close(sfd);
            continue;
        }

        LOG_LF("(request) %s", buffer);

        Request request(buffer);

        char *response = (char *)router.handle(&request);

        send(sfd, response, strlen(response), 0);

        close(sfd);
    }

    return 0;
}
