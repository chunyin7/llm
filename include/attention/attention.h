#ifndef ATTENTION
#define ATTENTION

#include <util/array.h>
#include <util/matrix.h>

// compute self-attention
Matrix *attention_forward(
  Matrix *token_embedding_matrix,
  Matrix *Wq,
  Matrix *Wk,
  Matrix *Wv,
  Matrix *Wo,
  double dropout_rate,
  size_t n_heads
);

void apply_causal_mask(Matrix *scores);

void apply_dropout_mask(Matrix *scores, double dropout_rate);

#endif
