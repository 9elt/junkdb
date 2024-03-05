#include "util.hpp"

#define UNKNOWN -1
#define GET 0
#define SET 1
#define HAS 2
#define ADD 3
#define REM 4

struct Request {
    unsigned long database;

    int action;

    long payload;

    Request(char *str);
};
