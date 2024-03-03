#include "database.hpp"
#include <stdio.h>

void Database::load() {
    char *path = this->path();

    FILE *f = fopen(path, "r");

    if (f == nullptr) {
        printf("Creating database %ld at %s\n", id, path);

        empty();

        if (dump() < 0) {
            delete[] path;
            id = -1;
        };

        return;
    }

    if (fread(this, sizeof(Database), 1, f) != 1) {
        printf("Cannot read database %ld at %s\n", id, path);

        delete[] path;
        id = -1;

        return;
    }

    printf("Loaded database %ld at %s, unique %ld, size %d\n", id, path, unique,
           buffer_end);

    delete[] path;
}

char *Database::path() {
    char *path = new char[128];
    sprintf(path, DATA_DIR "/%ld", id);

    return path;
}

void Database::empty() {
    unique = 0;
    buffer_end = 0;

    for (int i = 0; i < BUFFER_CAP; i++) {
        buffer[i] = -1;
    }
}

int Database::dump() {
    char *path = this->path();

    printf("Dumping database %ld at %s, unique %ld, size %d\n", id, path,
           unique, buffer_end);

    FILE *f = fopen(path, "w");

    if (f == nullptr) {
        printf("Dump failed, error opening file %s\n", path);

        delete[] path;

        return -1;
    }

    if (fwrite(this, sizeof(Database), 1, f) != 1) {
        printf("Dump failed, error writing to file %s\n", path);

        fclose(f);

        delete[] path;

        return -1;
    }

    fclose(f);

    delete[] path;

    return 0;
}

Database::Database() {}

Database::Database(long int _id) {
    id = _id;
    load();
}

bool Database::is(long int _unique) { return unique == _unique; }

void Database::set(long int _unique) {
    unique = _unique;

    dump();
}

void Database::add(long int hash) {
    if (buffer_end == BUFFER_CAP) {
        buffer_end = 0;
    }

    buffer[buffer_end++] = hash;

    dump();
}

bool Database::has(long int hash) {
    for (int i = 0; i < BUFFER_CAP; i++) {
        if (buffer[i] == hash) {
            return true;
        }
    }

    return false;
}

void Database::remove(long int hash) {
    for (int pop = 0; pop < BUFFER_CAP; pop++) {
        if (buffer[pop] == hash) {
            if (pop == buffer_end - 1) {
                ;
            } else if (pop < buffer_end) {
                for (int i = pop; i < buffer_end - 1; i++) {
                    buffer[i] = buffer[i + 1];
                }
            } else {
                for (int j = pop; j < BUFFER_CAP - 1; j++) {
                    buffer[j] = buffer[j + 1];
                }

                buffer[BUFFER_CAP - 1] = buffer[0];

                for (int j = 0; j < buffer_end - 1; j++) {
                    buffer[j] = buffer[j + 1];
                }
            }

            pop--;
            buffer_end--;
            buffer[buffer_end] = -1;
        }
    }

    printf("Removed (%ld)\n", hash);

    dump();
}
