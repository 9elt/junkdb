#include "active.hpp"
#include "request.hpp"

#define TRUE "1"
#define FALSE "0"

#define UNKNOWN_ACTION "-1"
#define UNREACHABLE_DATABASE "-2"
#define SERVER_ERROR "-3"

struct Router {
    Active *databases;

    char const *handle(Request *request);

    Router(Active *databases);
};
