#include "database.hpp"
#include <stdio.h>

void Database::load() {
    char *path = this->path();

    FILE *f = fopen(path, "r");

    if (f == nullptr) {
        LOG_LF("Creating %lu at %s", id, path);

        empty();

        if (dump() < 0) {
            delete[] path;
            id = DEFAULT_ID;
        };

        return;
    }

    if (fread(this, sizeof(Database), 1, f) != 1) {
        ERROR_LF("Cannot read %lu at %s", id, path);

        delete[] path;
        id = DEFAULT_ID;

        return;
    }

    LOG_LF("Loaded %lu from %s, status %lu tail %d", id, path, status,
           buffer_end);

    delete[] path;
}

char *Database::path() {
    char *path = new char[128];
    sprintf(path, DATA_DIR "/%lu", id);

    return path;
}

void Database::empty() {
    status = 0;
    buffer_end = 0;

    for (int i = 0; i < BUFFER_CAP; i++) {
        buffer[i] = -1;
    }
}

int Database::dump() {
    char *path = this->path();

    LOG_LF("Dumping %lu at %s, status %ld tail %d", id, path, status,
           buffer_end);

    FILE *f = fopen(path, "w");

    if (f == nullptr) {
        ERROR_LF("Dump failed, error opening file %s", path);

        delete[] path;

        return -1;
    }

    if (fwrite(this, sizeof(Database), 1, f) != 1) {
        ERROR_LF("Dump failed, error writing to file %s", path);

        fclose(f);

        delete[] path;

        return -1;
    }

    fclose(f);

    delete[] path;

    return 0;
}

Database::Database() {}

Database::Database(unsigned long _id) {
    id = _id == DEFAULT_ID ? _id + 1 : _id;
    load();
}

long Database::get() { return status; }

void Database::set(long _status) {
    status = _status;

    dump();
}

void Database::add(long hash) {
    if (buffer_end == BUFFER_CAP) {
        buffer_end = 0;
    }

    buffer[buffer_end++] = hash;

    dump();
}

bool Database::has(long hash) {
    for (int i = 0; i < BUFFER_CAP; i++) {
        if (buffer[i] == hash) {
            return true;
        }
    }

    return false;
}

void Database::remove(long hash) {
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

    dump();
}

void Database::clear() {
    char *path = this->path();

    LOG_LF("Deleting %lu at %s", id, path);

    std::remove(path);

    delete[] path;
}
