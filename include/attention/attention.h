#ifndef ATTENTION
#define ATTENTION

#include <util/array.h>
#include <util/matrix.h>

typedef struct {
  size_t n_heads;
  size_t d_model;
  Matrix *Wq;
  Matrix *Wk;
  Matrix *Wv;
  Matrix *Wo;
  double dropout_rate;
} Attention;

Attention *attention_init(size_t n_heads, size_t d_model, size_t dropout_rate);

void attention_free(Attention *attention);

// compute self-attention
Matrix *attention_forward(
  Attention *attention,
  Matrix *token_embedding_matrix
);

void apply_causal_mask(Matrix *scores);

void apply_dropout_mask(Matrix *scores, double dropout_rate);

#endif
