#include "config.hpp"

class Database {
    long int unique;

    int size;

    long int data[HASHES_CAP];

    void load();

    void empty();

    int dump();

    char *path();

  public:
    long int id;

    Database();

    Database(long int id);

    bool is(long int unique);

    void set(long int unique);

    void add(long int hash);

    bool has(long int hash);

    void remove(long int hash);
};
