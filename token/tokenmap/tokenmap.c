#include "tokenmap.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

// FNV-1a 64-bit hash function
static uint64_t fnv_1a(const Array *arr) {
    uint64_t h = 1469598103934665603ULL; // offset basis
    
    if (arr == NULL) {
        return h;
    }
    
    uint8_t *ids = (uint8_t *)arr->data;

    for (int i = 0; i < arr->len; i++) {
        h ^= ids[i];
        h *= 1099511628211ULL; // FNV prime
  }

  return h;
}

Map *map_init() {
  Map *map = malloc(sizeof(Map));
  map->cap = INIT_CAP;
  map->len = 0;

  map->entries = calloc(INIT_CAP, sizeof(Entry));

  for (size_t i = 0; i < map->cap; i++) {
    map->entries[i].occupied = 0;
  }

  return map;
}

void map_free(Map *map) {
    for (size_t i = 0; i < map->cap; i++) {
        if (map->entries[i].occupied) {
            free(map->entries[i].key.ids);
        }
    }

    free(map->entries);
    free(map);

    return;
}

static void map_resize(Map *map) {
  Entry *old = map->entries;
  size_t old_cap = map->cap;

  map->cap *= 2;
  map->entries = calloc(map->cap, sizeof(Entry));

  for (size_t i = 0; i < map->cap; i++) {
    map->entries[i].occupied = 0;
  }

  // rehash all entries
  for (size_t i = 0; i < old_cap; i++) {
    if (old[i].occupied) {
      uint64_t h = fnv_1a(old[i].key.ids) & (map->cap - 1);

      // linearly probe with open addressing
      while (map->entries[h].occupied) {
        h++;
        h &= map->cap - 1;
      }

      map->entries[h] = old[i];
    }
  }

  free(old);
  return;
}

long map_add(Map *map, Token key, long val) {
  if ((double) map->len / map->cap > LOAD_FACTOR) {
    map_resize(map);
  }

  uint64_t h = fnv_1a(key.ids) & (map->cap - 1);
  while (map->entries[h].occupied) {
    if (arr_cmp(map->entries[h].key.ids, key.ids) && map->entries[h].key.type == key.type) {
      return map->entries[h].val;
    }

    h++;
    h &= map->cap - 1;
  }

  // insert
  map->entries[h].key = key;
  map->entries[h].val = val;
  map->entries[h].occupied = 1;
  map->len++;
  return val;
}

void map_update(Map *map, Token key, long val) {
  if ((double) map->len / map->cap > LOAD_FACTOR) {
    map_resize(map);
  }

  uint64_t h = fnv_1a(key.ids) & (map->cap - 1);
  while (map->entries[h].occupied) {
    if (arr_cmp(map->entries[h].key.ids, key.ids) && map->entries[h].key.type == key.type) {
      map->entries[h].val = val;
    }

    h++;
    h &= map->cap - 1;
  }

  // insert
  map->entries[h].key = key;
  map->entries[h].val = val;
  map->entries[h].occupied = 1;
  map->len++;
}

long map_get(Map *map, Token key) {
  uint64_t h = fnv_1a(key.ids) & (map->cap - 1);
  while (map->entries[h].occupied) {
    if (arr_cmp(map->entries[h].key.ids, key.ids) && map->entries[h].key.type == key.type) {
      return map->entries[h].val;
    }

    h++;
    h &= map->cap - 1;
  }

  return -1;
}
