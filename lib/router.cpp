#include "router.hpp"

Router::Router(Active *_databases) { databases = _databases; }

char const *Router::handle(Request *request) {
    if (request->action == UNKNOWN) {
        return UNKNOW_ACTION;
    }

    Database *db = databases->get(request->database);

    if (db == nullptr) {
        return UNREACHABLE_DATABASE;
    }

    if (request->action == IS) {
        return db->is(request->payload) ? TRUE : FALSE;
    }

    if (request->action == SET) {
        db->set(request->payload);
        return TRUE;
    }

    if (request->action == HAS) {
        return db->has(request->payload) ? TRUE : FALSE;
    }

    if (request->action == ADD) {
        db->add(request->payload);
        return TRUE;
    }

    if (request->action == REM) {
        db->remove(request->payload);
        return TRUE;
    }

    return SERVER_ERROR;
}
