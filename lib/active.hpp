#include "database.hpp"
#include "util.hpp"

struct Active {
    int size;

    Database databases[ACTIVE_DATABASES];

    Active();

    Database *get(long int id);
};
