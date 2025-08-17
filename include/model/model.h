#ifndef MODEL
#define MODEL

#include <transformer/transformer.h>

typedef struct {
  Matrix *tok_emb;
  Matrix *pos_emb;
  TransformerBlock **blocks;
  LayerNorm *final_norm;
  Matrix *out;
  size_t n_layers;
  size_t context_len;
  double dropout_rate;
} Model;

Model *model_init(
  size_t n_layers,
  size_t n_heads,
  size_t d_model,
  size_t d_ff,
  double dropout_rate,
  double eps,
  size_t voc_size,
  size_t context_len
);

void model_free(Model *model);

Matrix *model_forward(Model *model, long *ids, size_t batch_size, size_t seq_len);

#endif
