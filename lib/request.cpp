#include "request.hpp"
#include <cstdio>

Request::Request(char *str) {
    char database_str[128];
    char action_str[4];
    char data_str[4096];

    int cursor = 0;
    int cursor_rel = 0;

    while (str[cursor] == ' ')
        cursor++;

    while (str[cursor] != ' ' && str[cursor] != '\0') {
        if (cursor_rel < 128)
            database_str[cursor_rel] = str[cursor];

        cursor++;
        cursor_rel++;
    }

    database_str[cursor_rel] = '\0';

    while (str[cursor] == ' ')
        cursor++;

    cursor_rel = 0;

    while (str[cursor] != ' ' && str[cursor] != '\0') {
        if (cursor_rel < 4)
            action_str[cursor_rel] = str[cursor];

        cursor++;
        cursor_rel++;
    }

    while (str[cursor] == ' ')
        cursor++;

    cursor_rel = 0;

    while (str[cursor] != '\0') {
        if (cursor_rel < 4096)
            data_str[cursor_rel] = str[cursor];

        cursor++;
        cursor_rel++;
    }

    data_str[cursor_rel] = '\0';

    database = hash(database_str);

    if (action_str[0] == 'I' && action_str[1] == 'S') {
        action = IS;
    } else if (action_str[0] == 'S' && action_str[1] == 'E' &&
               action_str[2] == 'T') {
        action = SET;
    } else if (action_str[0] == 'H' && action_str[1] == 'A' &&
               action_str[2] == 'S') {
        action = HAS;
    } else if (action_str[0] == 'A' && action_str[1] == 'D' &&
               action_str[2] == 'D') {
        action = ADD;
    } else if (action_str[0] == 'R' && action_str[1] == 'E' &&
               action_str[2] == 'M') {
        action = REM;
    } else {
        printf("Unknown action: '%s'\n", action_str);
        action = UNKNOWN;
    }

    payload = hash(data_str);
}
