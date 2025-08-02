#ifndef ATTENTION
#define ATTENTION

#include <util/array.h>
#include <util/matrix.h>

// compute self-attention
Matrix *forward_pass(
  Matrix *token_embedding_matrix,
  Matrix *Wq,
  Matrix *Wk,
  Matrix *Wv,
  float dropout_rate,
  size_t n_heads
);

void apply_causal_mask(Matrix *scores);

void apply_dropout_mask(Matrix *scores, float dropout_rate);

#endif
