#include <model/model.h>

Model *model_init(
  size_t n_layers,
  size_t n_heads,
  size_t d_model,
  size_t d_ff,
  double dropout_rate,
  double eps,
  size_t voc_size
) {
  Model *model = malloc(sizeof(Model));
  model->n_layers = n_layers;
  model->tok_emb = matrix_init(voc_size, d_model);
  model->pos_emb = matrix_init(voc_size, d_model);
  model->blocks = malloc(sizeof(TransformerBlock) * n_layers);

  for (size_t i = 0; i < n_layers; i++) {
    model->blocks[i] = transformer_block_init(
      n_heads,
      d_model,
      d_ff,
      dropout_rate,
      eps
    );
  }

  model->final_norm = layer_norm_init(eps, d_model);
  model->out = matrix_init(d_model, voc_size);

  return model;
}

void model_free(Model *model) {
  matrix_free(model->tok_emb);
  matrix_free(model->pos_emb);
  for (size_t i = 0; i < model->n_layers; i++) {
    transformer_block_free(model->blocks[i]);
  }
  free(model->blocks);
  layer_norm_free(model->final_norm);
  matrix_free(model->out);
  free(model);
}
