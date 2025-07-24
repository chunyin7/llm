#ifndef ATTENTION
#define ATTENTION

#include <util/array.h>

// naive self-attention scores
Array *compute_self_attention_scores(Array *vec);

// normalize to produce weights
void softmax(Array *scores);

// compute context vectors
// weighted sums of embeddings
Array *compute_context_mat(Array *weights, Array *embeddings, size_t dim);

#endif
