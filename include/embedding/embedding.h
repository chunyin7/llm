#ifndef EMBEDDING
#define EMBEDDING

#include <stdlib.h>
#include <util/array.h>

// emit randomized embedding matrix
// can also be used for pos embedding where voc_size = max_seq_len
Array *init_embedding(size_t dim, size_t voc_size);

// embed token ids into embedding matrix
Array *embed(Array *ids, Array *emb_mat, size_t dim);

#endif
