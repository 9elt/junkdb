#include "active.hpp"
#include "request.hpp"

#define FALSE "false"
#define TRUE "true"
#define OK "ok"

#define UNKNOWN_ACTION "unknown action"
#define UNREACHABLE_DATABASE "unreachable database"
#define SERVER_ERROR "server error"

struct Router {
    Active *databases;

    char const *handle(Request *request);

    Router(Active *databases);
};
