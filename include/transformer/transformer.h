#ifndef TRANSFORMER
#define TRANSFORMER

#include <transformer/feed_forward.h>
#include <transformer/layer_norm.h>
#include <util/array.h>
#include <util/matrix.h>
#include <transformer/attention.h>

typedef struct {
  Attention *attention;
  FeedForward *ff;
  LayerNorm *norm1;
  LayerNorm *norm2;
} TransformerBlock;

TransformerBlock *transformer_block_init(
  size_t n_heads,
  size_t d_model,
  size_t d_ff,
  size_t dropout_rate,
  double eps
);

void transformer_block_free(TransformerBlock *block);

Matrix *transformer_forward(
  TransformerBlock *block,
  Matrix *x
);

#endif
