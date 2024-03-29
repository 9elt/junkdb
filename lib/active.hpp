#include "database.hpp"
#include "util.hpp"

struct Active {
    int size;

    Database databases[ACTIVE_DATABASES];

    Active();

    Database *get(unsigned long id);

    void clear(unsigned long id);
};
