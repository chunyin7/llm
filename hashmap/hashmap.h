// a basic linear probing / open addressing hashmap to map string tokens to int ids (their insertion index)
#ifndef MAP
#define MAP

#define INIT_CAP 1024
#define LOAD_FACTOR 0.7

typedef struct {
    char *key; // string key
    int val; // int id value
} Entry;

// Hashmap struct
typedef struct {
    Entry *entries;
    size_t cap, len;
} Map;

void map_init();

void map_free(Map *map);

void map_get_or_add(Map *map, char *key);

#endif
