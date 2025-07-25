#ifndef TOKENIZER
#define TOKENIZER

#include "vocabulary.h"
#include <util/matrix.h>

Vocabulary *bpe(size_t voc_size, uint8_t *in, size_t len);

Array *tokenize(uint8_t *in, size_t len, Vocabulary *voc);

Array *encode(uint8_t *in, size_t len, Vocabulary *voc);

Array *decode(Array *ids, Vocabulary *voc);

// save token sequence (ids) to file
void save_token_sequence(Array *ids);

// load token sequence (ids) from file
void load_token_sequence(const char *path);

// embed token id sequence into an embedding matrix
Matrix *embed(Array *ids, Matrix *emb_mat);

#endif
