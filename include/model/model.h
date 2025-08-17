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
} Model;

Model *model_init(
  size_t n_layers,
  size_t n_heads,
  size_t d_model,
  size_t d_ff,
  double dropout_rate,
  double eps,
  size_t voc_size
);

void model_free(Model *model);

#endif
