#ifndef TOKENIZER
#define TOKENIZER

#include "vocabulary.h"

Vocabulary *bpe(size_t voc_size, uint8_t *in, size_t len);

Array *tokenize(uint8_t *in, size_t len, Vocabulary *voc);

Array *encode(uint8_t *in, size_t len, Vocabulary *voc);

Array *decode(Array *ids, Vocabulary *voc);

#endif
