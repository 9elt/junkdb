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
           size);

    delete[] path;
}

char *Database::path() {
    char *path = new char[128];
    sprintf(path, DATA_DIR "/%ld", id);

    return path;
}

void Database::empty() {
    unique = 0;
    size = 0;

    for (int i = 0; i < HASHES_CAP; i++) {
        data[i] = -1;
    }
}

int Database::dump() {
    char *path = this->path();

    printf("Dumping database %ld at %s, unique %ld, size %d\n", id, path,
           unique, size);

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
    if (size == HASHES_CAP) {
        size = 0;
    }

    data[size++] = hash;

    dump();
}

bool Database::has(long int hash) {
    for (int i = 0; i < HASHES_CAP; i++) {
        if (data[i] == hash) {
            return true;
        }
    }

    return false;
}

void Database::remove(long int hash) {
    for (int i = 0; i < size; i++) {
        if (data[i] == hash) {
            data[i] = -1;
            for (int j = i; j < size - 1; j++) {
                data[j] = data[j + 1];
            }
            size--;
        }
    }

    dump();
}
