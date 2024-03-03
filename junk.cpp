#define HASHES_CAP 4096

struct Database {
    long int id;

    int status;

    int hashes_length;

    int hashes[HASHES_CAP];

    Database() {
        id = 0;
        hashes_length = 0;

        for (int i = 0; i < HASHES_CAP; i++) {
            hashes[i] = -1;
        }
    }

    void push(int hash) {
        if (hashes_length == HASHES_CAP) {
            hashes_length = 0;
        }

        hashes[hashes_length++] = hash;
    }

    bool has(int hash) {
        for (int i = 0; i < HASHES_CAP; i++) {
            if (hashes[i] == hash) {
                return true;
            }
        }

        return false;
    }
};

long int hash(char *str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) ^ c;
    }

    return hash;
}

int main() { return 0; }
