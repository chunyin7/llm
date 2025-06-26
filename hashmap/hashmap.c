#include "hashmap.h"

// FNV-1a 64-bit hash function
static uint64_t fnv_1a(const char *str) {
    uint64_t h = 1469598103934665603ULL; // offset basis

    for (; *s != '\0'; s++) {
        h ^= (unsigned char)*s;
        h *= 1099511628211ULL; // FNV prime
    }

    return h;
}

void map_init() {
    Map *map = malloc(sizeof(Map));
    map->cap = INIT_CAP;
    map->len = 0;

    map->entries = calloc(INIT_CAP * sizeof(Entry));

    return;
}

void map_free(Map *map) {
    for (size_t i = 0; i < map->len; i++) {
        free(map->entries[i].key);
    }

    free(map->entries);
    free(map);

    return;
}

static void map_resize(Map *map) {
    Entry *old = map->entries;
    size_t old_cap = map->cap;

    map->cap *= 2;
    map->entries = calloc(map->cap * sizeof(Entry));

    // rehash all entries
    for (size_t i = 0; i < map->cap; i++) {
            uint64_t h = fnv_1a(old[i].key) & (map->cap - 1);

            // linearly probe with open addressing
            while (map->entries[h].key) {
                h++;
                h &= map->cap - 1
            }

            map->entries[h] = old[i];
        }
    }

    free(old);
    return;
}

void map_get_or_add(Map *map, char *key) {
    if ((double) map->len / map->cap > LOAD_FACTOR) {
        map_resize(map);
    }

    uint64_t h = fnv_1a(key) & (map->cap - 1);
    while (map->entries[h].key) {
        if (strcmp(map->entries[h].key, key) == 0) {
            return map->entries[h].val;
        }

        h++;
        h &= map->cap - 1;
    }

    // insert
    map->entries[h].key = strdup(key);
    map->entries[h].val = map->len;
    return map->len++;
}
