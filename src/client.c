#include "actions.h"
#include "config.h"
#include "log.h"
#include "types.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

const char *VERSION = "0.1.0";

const char *HELP =
    "usage: %s <database> [options] <payload> [options]\n"
    "       %s <request> [options] --raw [options]\n\n"
    "--strip,   -s    strip response status\n"
    "--raw            execute raw request\n"
    "--get,     -G    get the database status\n"
    "--set,     -S    set the database status\n"
    "--has,     -H    check if the database contains the payload\n"
    "--add,     -A    add the payload to the database\n"
    "--remove,  -R    remove the payload from the database\n"
    "--delete,  -D    delete the datbase\n"
    "--help,    -h    show this help message\n"
    "--version, -V    show junkdb-cli version\n";

int main(int argc, char **argv) {
    char *program = argv[0];

    if (argc < 2) {
        FAIL_LF("No arguments provided, check %s --help", program);
    }

    char *request;

    char *dbname = NULL;
    char *unmarked = NULL;

    int action = UNKNOWN;

    bool exect_raw = false;
    bool strip_response_status = false;

    for (int i = 1; i < argc; i++) {
#define ARG(full, short)                                                       \
    (strcmp(argv[i], full) == 0 || strcmp(argv[i], short) == 0)

        if (ARG("--strip", "-s")) {
            strip_response_status = true;
        } else if (ARG("--raw", "-r")) {
            exect_raw = true;
        } else if (ARG("--get", "-G")) {
            action = GET;
        } else if (ARG("--set", "-S")) {
            action = SET;
        } else if (ARG("--has", "-H")) {
            action = HAS;
        } else if (ARG("--add", "-A")) {
            action = ADD;
        } else if (ARG("--remove", "-R")) {
            action = REM;
        } else if (ARG("--delete", "-D")) {
            action = DEL;
        } else if (ARG("--help", "-h")) {
            LOG_F(HELP, program, program);
            return 0;
        } else if (ARG("--version", "-V")) {
            LOG_LF("junkdb-cli version %s", VERSION);
            return 0;
        } else {
            unmarked = argv[i];
            if (i == 1) {
                dbname = argv[i];
            }
            continue;
        }
    }

    if (exect_raw) {
        if (unmarked == NULL) {
            FAIL_LF("No request provided, check %s --help", program);
        }

        request = unmarked;
    } else {
        if (action == UNKNOWN) {
            FAIL_LF("No action provided, check %s --help", program);
        }

        if (dbname == NULL) {
            FAIL_LF("No database name provided, check %s --help", program);
        }

        if ((action == SET || action == HAS || action == ADD ||
             action == REM) &&
            (unmarked == NULL || unmarked == dbname)) {
            FAIL_LF("No payload provided, check %s --help", program);
        }

        request = (char *)malloc((action == DEL || action == GET
                                      ? strlen(dbname) + 5
                                      : strlen(unmarked) + strlen(dbname) + 5) *
                                 sizeof(char));

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

    char buffer[128] = {0};

    int fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd == -1) {
        FAIL_L("Cannot open socket");
    }

    struct sockaddr_in serv_addr;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(PORT);

    if (connect(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        FAIL_L("Cannot connect to the server");
    }

    send(fd, request, strlen(request), 0);

    while (recv(fd, buffer, sizeof(buffer), 0) > 0) {
        if (strip_response_status) {
            // NOTE: 'OK ' or 'ERR
            int offset = buffer[0] == 'O' ? 3 : 4;

            if (buffer[offset - 1] != '\0') {
                LOG_F("%s", buffer + offset);
            }
        } else {
            LOG_LF("%s", buffer);
        }
        bzero(buffer, 3);
    }

    return 0;
}
