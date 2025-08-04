#include <transformer/attention.h>
#include <transformer/transformer.h>

TransformerBlock *transformer_block_init(
  size_t n_heads,
  size_t d_model,
  size_t d_ff,
  size_t dropout_rate,
  double eps
) {
  TransformerBlock *block = malloc(sizeof(TransformerBlock));
  block->attention = attention_init(n_heads, d_model, dropout_rate);
  block->ff = feed_forward_init(d_model);
  block->norm1 = layer_norm_init(eps, d_model);
  block->norm2 = layer_norm_init(eps, d_model);

  return block;
}

void transformer_block_free(TransformerBlock *block) {
  attention_free(block->attention);
  feed_forward_free(block->ff);
  layer_norm_free(block->norm1);
  layer_norm_free(block->norm2);
  free(block);
}

Matrix *transformer_forward(
  TransformerBlock *block,
  Matrix *x
) {
  Matrix *shortcut1 = matrix_dup(x);
  Matrix *x_norm1 = layer_norm_forward(block->norm1, x);
  Matrix *x_att = attention_forward(block->attention, x_norm1);
  apply_dropout_mask(x_att, block->attention->dropout_rate);
  Matrix *out1 = matrix_add(x_att, shortcut1);

  matrix_free(shortcut1);
  matrix_free(x_norm1);
  matrix_free(x_att);

  Matrix *shortcut2 = matrix_dup(out1);
  Matrix *x_norm2 = layer_norm_forward(block->norm2, out1);
  Matrix *x_ff = feed_forward_forward(block->ff, x_norm2);
  apply_dropout_mask(x_ff, block->attention->dropout_rate);
  Matrix *x_out = matrix_add(x_ff, shortcut2);

  matrix_free(shortcut2);
  matrix_free(x_norm2);
  matrix_free(x_ff);
  
  return x_out;
}
