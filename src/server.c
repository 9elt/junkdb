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

#define DEFAULT_ID 5381

unsigned long hash(char *str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) ^ c;
    }

    return hash;
}

typedef struct Database {
    unsigned long id;
    long status;
    int flag;
    long buffer[BUFFER_CAP];
} Database;

char *database_path(unsigned long id) {
    char *path = (char *)malloc(256 * sizeof(char));

    sprintf(path, DATA_DIR "/%lu.junk", id);

    return path;
}

void database_empty(Database *database) {
    database->status = 0;
    database->flag = 0;

    for (int i = 0; i < BUFFER_CAP; i++) {
        database->buffer[i] = -1;
    }
}

int database_dump(Database *database) {
    char *path = database_path(database->id);

    FILE *f = fopen(path, "w");

    if (f == NULL) {
        free(path);

        return -1;
    }

    if (fwrite(database, sizeof(Database), 1, f) != 1) {
        fclose(f);
        free(path);

        return -1;
    }

    fclose(f);
    free(path);

    return 0;
}

Database database_load(unsigned long id) {
    Database database;
    database.id = id == DEFAULT_ID ? id + 1 : id;

    char *path = database_path(database.id);

    FILE *f = fopen(path, "r");

    if (f == NULL) {
        database_empty(&database);

        if (database_dump(&database) < 0) {
            database.id = DEFAULT_ID;
        }

        free(path);

        return database;
    }

    if (fread(&database, sizeof(Database), 1, f) != 1) {
        database.id = DEFAULT_ID;
    }

    fclose(f);
    free(path);

    return database;
}

void database_set(Database *database, long status) {
    database->status = status;

    database_dump(database);
}

bool database_has(Database *database, long hash) {
    for (int i = 0; i < BUFFER_CAP; i++) {
        if (database->buffer[i] == hash) {
            return false;
        }
    }

    return true;
}

void database_add(Database *database, long hash) {
    database->buffer[database->flag] = hash;
    database->flag = (database->flag + 1) % BUFFER_CAP;

    database_dump(database);
}

void database_remove(Database *database, long hash) {
    for (int pop = 0; pop < BUFFER_CAP; pop++) {
        if (database->buffer[pop] == hash) {
            if (pop == database->flag - 1) {
                ;
            } else if (pop < database->flag) {
                for (int i = pop; i < database->flag - 1; i++) {
                    database->buffer[i] = database->buffer[i + 1];
                }
            } else {
                for (int j = pop; j < BUFFER_CAP - 1; j++) {
                    database->buffer[j] = database->buffer[j + 1];
                }

                database->buffer[BUFFER_CAP - 1] = database->buffer[0];

                for (int j = 0; j < database->flag - 1; j++) {
                    database->buffer[j] = database->buffer[j + 1];
                }
            }

            pop--;
            database->flag--;
            database->buffer[database->flag] = -1;
        }
    }

    database_dump(database);
}

void database_delete(Database *database) {
    char *path = database_path(database->id);

    remove(path);
    free(path);
}

typedef struct Pool {
    int size;
    Database databases[ACTIVE_DATABASES];
} Pool;

Database *pool_get(Pool *pool, unsigned long id) {
    for (int i = 0; i < pool->size; i++) {
        if (pool->databases[i].id == id) {
            return &pool->databases[i];
        }
    }

    Database database = database_load(id);

    if (database.id == DEFAULT_ID) {
        return NULL;
    }

    if (pool->size == ACTIVE_DATABASES) {
        pool->size = 0;
    }

    pool->databases[pool->size] = database;

    return &pool->databases[pool->size++];
}

void pool_delete(Pool *pool, unsigned long id) {
    for (int i = 0; i < pool->size; i++) {
        if (pool->databases[i].id == id) {
            database_delete(&pool->databases[i]);

            pool->size--;
            for (int j = i; j < pool->size; j++) {
                pool->databases[j] = pool->databases[j + 1];
            }
        }
    }
}

typedef struct Request {
    unsigned long database_id;
    int action;
    long payload;
} Request;

Request request_parse(char *buffer) {
    Request request;

    char database_name[128];
    char action_name[4];
    char body[4096];

    int request_cursor = 0;
    int section_cursor = 0;

#define NOT_SPACE buffer[request_cursor] != ' '
#define NOT_END buffer[request_cursor] != '\0'

#define NEXT_SECTION                                                           \
    while (buffer[request_cursor] == ' ')                                      \
        request_cursor++;                                                      \
    section_cursor = 0;

    NEXT_SECTION;

    while (NOT_SPACE && NOT_END) {
        if (section_cursor < 128) {
            database_name[section_cursor] = buffer[request_cursor];
        }
        request_cursor++;
        section_cursor++;
    }

    database_name[section_cursor] = '\0';

    NEXT_SECTION;

    while (NOT_SPACE && NOT_END) {
        if (section_cursor < 4) {
            action_name[section_cursor] = buffer[request_cursor];
        }
        request_cursor++;
        section_cursor++;
    }

    NEXT_SECTION;

    while (NOT_END) {
        if (section_cursor < 4096) {
            body[section_cursor] = buffer[request_cursor];
        }
        request_cursor++;
        section_cursor++;
    }

    body[section_cursor] = '\0';

    request.database_id = hash(database_name);

#define IS_ACTION(c1, c2, c3)                                                  \
    action_name[0] == c1 &&action_name[1] == c2 &&action_name[2] == c3

    if (IS_ACTION('G', 'E', 'T')) {
        request.action = GET;
    } else if (IS_ACTION('S', 'E', 'T')) {
        request.action = SET;
    } else if (IS_ACTION('H', 'A', 'S')) {
        request.action = HAS;
    } else if (IS_ACTION('A', 'D', 'D')) {
        request.action = ADD;
    } else if (IS_ACTION('R', 'E', 'M')) {
        request.action = REM;
    } else if (IS_ACTION('D', 'E', 'L')) {
        request.action = DEL;
    } else {
        request.action = UNKNOWN;
    }

    request.payload = request.action == GET   ? 0
                      : request.action == SET ? atol(body)
                                              : hash(body);

    return request;
}

#define FALSE "OK false"
#define TRUE "OK true"
#define OK "OK"

#define UNKNOWN_ACTION "ERR unknown action"
#define UNREACHABLE_DATABASE "ERR unreachable database"
#define SERVER_ERROR "ERR server error"

char *request_handle(Pool *pool, Request *request) {
    Database *database = pool_get(pool, request->database_id);

    if (database == NULL) {
        return UNREACHABLE_DATABASE;
    }

    switch (request->action) {
    case GET: {
        long res = database->status;

        char *response = (char *)malloc((res / 10 + 5) * sizeof(char));

        response[0] = 'O';
        response[1] = 'K';
        response[2] = ' ';

        sprintf(response + 3, "%ld", res);

        return response;
    }
    case SET: {
        database_set(database, request->payload);
        return OK;
    }
    case HAS: {
        return database_has(database, request->payload) ? TRUE : FALSE;
    }
    case ADD: {
        database_add(database, request->payload);
        return OK;
    }
    case REM: {
        database_remove(database, request->payload);
        return OK;
    }
    case DEL: {
        pool_delete(pool, request->database_id);
        return OK;
    }
    case UNKNOWN: {
        return UNKNOWN_ACTION;
    }
    default: {
        return SERVER_ERROR;
    }
    }
}

int main(int argc, char **argv) {
    const in_addr_t LOCALHOST = inet_addr("127.0.0.1");

    const char *VERSION = "0.1.0";

    const char *HELP = "usage: %s [options]\n"
                       "--help,    -h    show this help message\n"
                       "--version, -V    show version\n";

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

    Pool pool;

    int fd;
    int sfd;
    int opt = 1;

    char buffer[4096];

    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;

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

    if (bind(fd, (struct sockaddr *)&serv_addr, socklen) < 0) {
        FAIL_L("Socket bind failed");
    }

    LOG_LF("Server started at port %d", PORT);

    if (listen(fd, 32) == -1) {
        close(fd);
        FAIL_L("Socket listen failed");
    }

    while (true) {
        sfd = accept(fd, (struct sockaddr *)&cli_addr, &socklen);

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

        Request request = request_parse(buffer);

        char *response = request_handle(&pool, &request);

        send(sfd, response, strlen(response), 0);

        free(response);
        close(sfd);
    }

    return 0;
}