#include <transformer/attention.h>
#include <token/tokenizer.h>
#include <model/model.h>

Model *model_init(
  size_t n_layers,
  size_t n_heads,
  size_t d_model,
  size_t d_ff,
  double dropout_rate,
  double eps,
  size_t voc_size,
  size_t context_len
) {
  Model *model = malloc(sizeof(Model));
  model->n_layers = n_layers;
  model->context_len = context_len;
  model->dropout_rate = dropout_rate;
  model->tok_emb = matrix_init(voc_size, d_model);
  model->pos_emb = matrix_init(context_len, d_model);
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

Matrix *model_forward(Model *model, long *ids, size_t batch_size, size_t seq_len) {
  // convert the ids to embeddings
  Matrix *tok_emb = embed(ids, model->tok_emb);

  // truncate pos embeddings to be seq_len
  Matrix *pos_emb = matrix_init(seq_len, model->pos_emb->cols);
  for (size_t i = 0; i < seq_len; i++) {
    for (size_t j = 0; j < model->pos_emb->cols; j++) {
      matrix_set(pos_emb, i, j, matrix_get(model->pos_emb, i, j));
    }
  }

  Matrix *combined = matrix_add(tok_emb, pos_emb);

  apply_dropout_mask(combined, model->dropout_rate);

  Matrix *x = combined;
  Matrix *prev = NULL;
  for (size_t i = 0; i < model->n_layers; i++) {
    prev = x;
    x = transformer_forward(model->blocks[i], x);
    matrix_free(prev);
  }

  Matrix *normed = layer_norm_forward(model->final_norm, x);
  Matrix *logits = matrix_multiply(normed, model->out);

  matrix_free(tok_emb);
  matrix_free(combined);
  matrix_free(normed);
  matrix_free(x);

  return logits;
}
