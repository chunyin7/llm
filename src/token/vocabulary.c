#include <token/vocabulary.h>
#include <stdio.h>

Vocabulary *voc_init() {
  Vocabulary *voc = malloc(sizeof(Vocabulary));
  voc->t2i = map_init();
  voc->i2t = arr_init(sizeof(Token));

  return voc;
}

void voc_add(Vocabulary *voc, Token tok) {
  int r = map_add(voc->t2i, tok, voc->t2i->len);
  if (r == voc->i2t->len) {
    arr_append(voc->i2t, &tok);
  }
}

void voc_free(Vocabulary *voc) {
  // The hashmap owns the tokens and will free them
  map_free(voc->t2i);
  // i2t array just has references, so only free the array itself
  arr_free(voc->i2t);
  free(voc);
}

// FNV-1a 64-bit hash function
static uint64_t fnv_1a(const Array *arr) {
    uint64_t h = 1469598103934665603ULL; // offset basis
    
    if (arr == NULL) {
        return h;
    }
    
    uint16_t *ids = (uint16_t *)arr->data;

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
      return;
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

void voc_save(Vocabulary *voc) {
  FILE *out = fopen("bin/vocabulary", "wb+");

  // header
  uint32_t len = (uint32_t)voc->i2t->len;
  fwrite(&len, sizeof(uint32_t), 1, out); // write the number of tokens
  
  // write each token
  for (uint32_t i = 0; i < len; i++) {
    Token tok = ((Token *)voc->i2t->data)[i];
    uint8_t type = tok.type; // 0, 1, 2
    uint16_t size = tok.ids->len;
    
    fwrite(&type, sizeof(uint8_t), 1, out);
    fwrite(&size, sizeof(uint16_t), 1, out);
    fwrite(tok.ids->data, sizeof(uint16_t), size, out);
  }

  fclose(out);
}
