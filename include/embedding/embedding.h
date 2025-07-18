#include <stdlib.h>
#include <arr/array.h>

// emit randomized embedding matrix
Array *init_embedding(size_t dim, size_t voc_size);

// embed token ids into embedding matrix
Array *embed(Array *ids, Array *emb_mat, size_t dim);
