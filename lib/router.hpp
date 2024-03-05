#include "active.hpp"
#include "request.hpp"

#define FALSE "OK false"
#define TRUE "OK true"
#define OK "OK"

#define UNKNOWN_ACTION "ERR unknown action"
#define UNREACHABLE_DATABASE "ERR unreachable database"
#define SERVER_ERROR "ERR server error"

struct Router {
    Active *databases;

    char const *handle(Request *request);

    Router(Active *databases);
};
