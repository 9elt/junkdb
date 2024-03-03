#include "database.hpp"
#include <stdio.h>

void Database::load() {
    char *tmp_path = new char[128];
    sprintf(tmp_path, DATA_DIR "/%ld", id);

    FILE *f = fopen(tmp_path, "r");

    if (f == nullptr || fread(this, sizeof(Database), 1, f) != 1) {
        printf("Creading database %ld at %s\n", id, tmp_path);

        path = tmp_path;
        empty();

        if (dump() < 0) {
            delete[] path;
            path = nullptr;
            id = -1;
        };

        return;
    }

    path = tmp_path;
}

void Database::empty() {
    unique = 0;
    size = 0;

    for (int i = 0; i < HASHES_CAP; i++) {
        data[i] = -1;
    }
}

int Database::dump() {
    FILE *f = fopen(path, "w");

    if (f == nullptr) {
        printf("Dump failed, error opening file %s\n", path);
        return -1;
    }

    for (int i = 0; i < size; i++) {
        fwrite(this, sizeof(Database), 1, f);
    }

    fclose(f);

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
            for (int j = i; j < size - 1; j++) {
                data[j] = data[j + 1];
            }
            size--;
        }
    }

    dump();
}

Database::~Database() { delete[] path; }
