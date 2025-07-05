#ifndef TOKEN
#define TOKEN

#include <stdlib.h>
#include "tokenmap/tokenmap.h"
#include "../arr/array.h"

typedef struct {
  Map *t2i;
  Array *i2t;
} Vocabulary;

Vocabulary *voc_init();

void build_voc(Array *tl, Vocabulary *voc);

void tokenize(Array *tokens, char *str, size_t len);

Array *encode(char *str, Vocabulary *voc);

Array *decode(Array *ids, Vocabulary *voc);

void voc_free(Vocabulary *voc);

#endif
