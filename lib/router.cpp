#include "router.hpp"
#include <stdio.h>

Router::Router(Active *_databases) { databases = _databases; }

char const *Router::handle(Request *request) {
    Database *db = databases->get(request->database);

    if (db == nullptr) {
        return UNREACHABLE_DATABASE;
    }

    switch (request->action) {
    case GET: {
        long res = db->get();
        char *rstr = new char[res / 10 + 5];
        rstr[0] = 'O';
        rstr[1] = 'K';
        rstr[2] = ' ';
        sprintf(rstr + 3, "%ld", res);
        return rstr;
    }
    case SET: {
        db->set(request->payload);
        return OK;
    }
    case HAS: {
        return db->has(request->payload) ? TRUE : FALSE;
    }
    case ADD: {
        db->add(request->payload);
        return OK;
    }
    case REM: {
        db->remove(request->payload);
        return OK;
    }
    case DEL: {
        db->clear();
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
