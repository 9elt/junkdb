#include "active.hpp"

Active::Active() { size = 0; }

Database *Active::get(long int id) {
    for (int i = 0; i < size; i++) {
        if (databases[i].id == id) {
            return &databases[i];
        }
    }

    Database init = Database(id);

    if (init.id == -1) {
        return nullptr;
    }

    if (size == ACTIVE_DATABASES) {
        size = 0;
    }

    databases[size] = init;

    return &databases[size++];
}
