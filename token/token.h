#ifndef TOKEN
#define TOKEN

#include <stdlib.h>
#include "../hashmap/hashmap.h"

typedef struct {
  char **data;
  size_t len, cap;
} TokenList;

typedef struct {
  long *data;
  size_t len, cap;
} IDList;

typedef struct {
  Map *t2i;
  TokenList *i2t;
} Vocabulary;

TokenList *tl_init();

Vocabulary *voc_init();

void build_voc(TokenList *tl, Vocabulary *voc);

void tl_add(TokenList *tl, char *tok, size_t tok_len);

void tl_free(TokenList *tl);

void tokenize(TokenList *tl, char *str, size_t len);

void tl_print(TokenList *tl);

void voc_print(Map *voc);

IDList *encode(char *str, Vocabulary *voc);

char *decode(IDList *ids, Vocabulary *voc);

void id_print(IDList *ids);

void id_free(IDList *ids);

#endif
