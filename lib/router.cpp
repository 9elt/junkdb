#include "router.hpp"
#include <stdio.h>

Router::Router(Active *_databases) { databases = _databases; }

char const *Router::handle(Request *request) {
    if (request->action == UNKNOWN) {
        return UNKNOWN_ACTION;
    }

    Database *db = databases->get(request->database);

    if (db == nullptr) {
        return UNREACHABLE_DATABASE;
    }

    if (request->action == GET) {
        long res = db->get();
        char *rstr = new char[res / 10 + 5];
        rstr[0] = 'O';
        rstr[1] = 'K';
        rstr[2] = ' ';
        sprintf(rstr + 3, "%ld", res);
        return rstr;
    }

    if (request->action == SET) {
        db->set(request->payload);
        return OK;
    }

    if (request->action == HAS) {
        return db->has(request->payload) ? TRUE : FALSE;
    }

    if (request->action == ADD) {
        db->add(request->payload);
        return OK;
    }

    if (request->action == REM) {
        db->remove(request->payload);
        return OK;
    }

    return SERVER_ERROR;
}
