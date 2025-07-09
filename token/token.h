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

Vocabulary *bpe(size_t voc_size, uint8_t *in, size_t len);

void tokenize(Array *tokens, char *str, size_t len, Vocabulary *voc);

Array *encode(char *str, Vocabulary *voc);

Array *decode(Array *ids, Vocabulary *voc);

void voc_free(Vocabulary *voc);

#endif
