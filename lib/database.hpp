#include "config.hpp"

#define DEFAULT_ID 5381

class Database {
    long status;

    int buffer_end;

    long buffer[BUFFER_CAP];

    void load();

    void empty();

    int dump();

    char *path();

  public:
    unsigned long id;

    Database();

    Database(unsigned long id);

    long get();

    void set(long status);

    void add(long hash);

    bool has(long hash);

    void remove(long hash);
};
