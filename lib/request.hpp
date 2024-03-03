#include "util.hpp"

#define UNKNOWN -1
#define IS 0
#define SET 1
#define HAS 2
#define ADD 3
#define REM 4

struct Request {
    long int database;

    int action;

    long int payload;

    Request(char *str);
};
