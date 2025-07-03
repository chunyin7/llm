#ifndef TOKEN
#define TOKEN

#include <stdlib.h>
#include "../hashmap/hashmap.h"
#include "../arr/array.h"

typedef struct {
  Map *t2i;
  Array *i2t;
} Vocabulary;

Vocabulary *voc_init();

void build_voc(Array *tl, Vocabulary *voc);

void tokenize(Array *tokens, char *str, size_t len);

Array *encode(char *str, Vocabulary *voc);

char *decode(Array *ids, Vocabulary *voc);

#endif
