#include "active.hpp"

Active::Active() { size = 0; }

Database *Active::get(unsigned long id) {
    for (int i = 0; i < size; i++) {
        if (databases[i].id == id) {
            return &databases[i];
        }
    }

    Database init = Database(id);

    if (init.id == DEFAULT_ID) {
        return nullptr;
    }

    if (size == ACTIVE_DATABASES) {
        size = 0;
    }

    databases[size] = init;

    return &databases[size++];
}

void Active::clear(unsigned long id) {
    for (int i = 0; i < size; i++) {
        if (databases[i].id == id) {
            databases[i].clear();
            size--;
            for (int j = i; j < size; j++) {
                databases[j] = databases[j + 1];
            }
        }
    }
}
