// a basic linear probing / open addressing hashmap to map token keys to int values

#ifndef MAP
#define MAP

#define INIT_CAP 1024
#define LOAD_FACTOR 0.7

#include <stdint.h>
#include <stdlib.h>
#include "../../arr/array.h"

// types of tokens
typedef enum {
  RAW, // bytes
  EOW, // SPECIAL: end of word
  EOS, // SPECIAL: end of sequence
  UNK,
} TokenType;

typedef struct {
  Array *bytes; // dynamic byte array (uint8_t)
  TokenType type;
} Token;

// key value pair
typedef struct {
  Token key; // string key
  long val; // int id value
  int occupied; // boolean
} Entry;

// Hashmap struct
typedef struct {
    Entry *entries;
    size_t cap, len;
} Map;

Map *map_init();

void map_free(Map *map);

// add an entry into the hashmap, do nothing if the key already exists
long map_add(Map *map, Token key, long val);

// update an existing entry in the hashmap, add it if the key doesn't exist yet
void map_update(Map *map, Token key, long val);

// retrieve the value for a key, return -1 if not found -> -1 should never be a value in this case
long map_get(Map *map, Token key);

#endif
