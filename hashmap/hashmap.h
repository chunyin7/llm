// a basic linear probing / open addressing hashmap to map string tokens to int ids (their insertion index)
#ifndef MAP
#define MAP

#define INIT_CAP 1024
#define LOAD_FACTOR 0.7

#include <stdio.h>

// key value pair
typedef struct {
    char *key; // string key
    long val; // int id value
} Entry;

// Hashmap struct
typedef struct {
    Entry *entries;
    size_t cap, len;
} Map;

Map *map_init();

void map_free(Map *map);

// add an entry into the hashmap, do nothing if the key already exists
long map_add(Map *map, char *key, long val);

// update an existing entry in the hashmap, add it if the key doesn't exist yet
void map_update(Map *map, char *key, long val);

// retrieve the value for a key, return -1 if not found -> -1 should never be a value in this case
long map_get(Map *map, char *key);

#endif
