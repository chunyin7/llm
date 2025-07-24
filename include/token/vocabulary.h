#ifndef VOCAB
#define VOCAB

#define INIT_CAP 1024
#define LOAD_FACTOR 0.7

#include <stdint.h>
#include <stdlib.h>
#include <util/array.h>

// types of tokens
typedef enum {
  RAW, // bytes
  EOW, // SPECIAL: end of word
  EOS, // SPECIAL: end of sequence
} TokenType;

typedef struct {
  Array *ids; // dynamic byte array (uint16_t)
  TokenType type;
} Token;

// key value pair
typedef struct {
  Token key; // string key
  long val; // int id value
  int occupied; // boolean
} Entry;

// Hashmap struct, maps tokens to some int value
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

typedef struct {
  Map *t2i; // token to id
  Array *i2t; // id to token via index
} Vocabulary;

Vocabulary *voc_init();

void voc_free(Vocabulary *voc);

// add a token to the vocabulary
void voc_add(Vocabulary *voc, Token tok);

// export the vocabulary to a binary file
void voc_save(Vocabulary *voc);

// load the vocabulary from a binary file
Vocabulary *voc_load(const char *path);

#endif
