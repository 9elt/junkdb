#include "config.hpp"
#include "router.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

const char *VERSION = "0.1.0";

const char *HELP =
    "usage: %s <db-name> [options] <payload>\n"
    "       %s <request> --raw [options]\n\n"
    "  --strip,   -s    strip response status\n"
    "  --raw            execute raw request\n"
    "  --get,     -G    get the database status\n"
    "  --set,     -S    set the database status\n"
    "  --has,     -H    check if the database contains the payload\n"
    "  --add,     -A    add the payload to the database\n"
    "  --rem,     -R    remove the payload from the database\n"
    "  --del,     -D    delete the datbase\n"
    "  --help,    -h    show this help message\n"
    "  --version, -V    show junkdb-cli version\n";

int main(int argc, char **argv) {
    char *program = argv[0];

    if (argc < 2) {
        fprintf(stderr, "No arguments provided, check %s --help\n", program);
        return 1;
    }

    char *request;

    char *dbname = nullptr;
    char *unmarked = nullptr;

    int action = UNKNOWN;

    bool exect_raw = false;
    bool strip_response_status = false;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--strip") == 0 || strcmp(argv[i], "-s") == 0) {
            strip_response_status = true;
        } else if (strcmp(argv[i], "--raw") == 0) {
            exect_raw = true;
        } else if (strcmp(argv[i], "--get") == 0 ||
                   strcmp(argv[i], "-G") == 0) {
            action = GET;
        } else if (strcmp(argv[i], "--set") == 0 ||
                   strcmp(argv[i], "-S") == 0) {
            action = SET;
        } else if (strcmp(argv[i], "--has") == 0 ||
                   strcmp(argv[i], "-H") == 0) {
            action = HAS;
        } else if (strcmp(argv[i], "--add") == 0 ||
                   strcmp(argv[i], "-A") == 0) {
            action = ADD;
        } else if (strcmp(argv[i], "--rem") == 0 ||
                   strcmp(argv[i], "-R") == 0) {
            action = REM;
        } else if (strcmp(argv[i], "--del") == 0 ||
                   strcmp(argv[i], "-D") == 0) {
            action = DEL;
        } else if (strcmp(argv[i], "--help") == 0 ||
                   strcmp(argv[i], "-h") == 0) {
            printf(HELP, program, program);
            return 0;
        } else if (strcmp(argv[i], "--version") == 0 ||
                   strcmp(argv[i], "-V") == 0) {
            printf("junkdb-cli version %s\n", VERSION);
            return 0;
        } else if (strcmp(argv[i], "-") == 0) {
            fprintf(stderr, "Invalid argument '%s'\n", argv[i]);
            return 1;
        } else {
            unmarked = argv[i];
            if (i == 1) {
                dbname = argv[i];
            }
        }
    }

    if (exect_raw) {
        if (unmarked == nullptr) {
            fprintf(stderr, "No request provided, check %s --help\n", program);
            return 1;
        }

        request = unmarked;
    } else {
        if (action == UNKNOWN) {
            fprintf(stderr, "No action provided, check %s --help\n", program);
            return 1;
        }

        if (dbname == nullptr) {
            fprintf(stderr, "No database name provided, check %s --help\n",
                    program);
            return 1;
        }

        if ((action == SET || action == HAS || action == ADD ||
             action == REM) &&
            (unmarked == nullptr || unmarked == dbname)) {
            fprintf(stderr, "No payload provided, check %s --help\n", program);
            return 1;
        }

        int request_size = action == DEL || action == GET
                               ? strlen(dbname) + 5
                               : strlen(unmarked) + strlen(dbname) + 5;

        request = new char[request_size]{0};

        switch (action) {
        case GET:
            sprintf(request, "%s GET", dbname);
            break;
        case SET:
            sprintf(request, "%s SET %s", dbname, unmarked);
            break;
        case HAS:
            sprintf(request, "%s HAS %s", dbname, unmarked);
            break;
        case ADD:
            sprintf(request, "%s ADD %s", dbname, unmarked);
            break;
        case REM:
            sprintf(request, "%s REM %s", dbname, unmarked);
            break;
        case DEL:
            sprintf(request, "%s DEL", dbname);
            break;
        }
    }

    char buffer[128]{0};

    int fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd == -1) {
        fprintf(stderr, "Cannot open socket\n");
        return 1;
    }

    sockaddr_in serv_addr;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(PORT);

    if (connect(fd, (sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        fprintf(stderr, "Cannot connect to the server\n");
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
