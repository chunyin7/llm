#include <attention/attention.h>
#include <util/array.h>
#include <util/matrix.h>
#include <util/tensor.h>
#include <math.h>

Attention *attention_init(size_t n_heads, size_t d_model, size_t dropout_rate) {
  Attention *attention = malloc(sizeof(Attention));
  attention->n_heads = n_heads;
  attention->d_model = d_model;
  attention->Wq = matrix_init(d_model, d_model);
  attention->Wk = matrix_init(d_model, d_model);
  attention->Wv = matrix_init(d_model, d_model);
  attention->Wo = matrix_init(d_model, d_model);
  attention->dropout_rate = dropout_rate;
  return attention;
}

void attention_free(Attention *attention) {
  matrix_free(attention->Wq);
  matrix_free(attention->Wk);
  matrix_free(attention->Wv);
  matrix_free(attention->Wo);
  free(attention);
}



Matrix *attention_forward(
  Attention *attention,
  Matrix *token_embedding_matrix
) {
  // destructure attention block
  Matrix *Wq = attention->Wq;
  Matrix *Wk = attention->Wk;
  Matrix *Wv = attention->Wv;
  Matrix *Wo = attention->Wo;
  size_t n_heads = attention->n_heads;
  size_t dropout_rate = attention->dropout_rate;

  // compute attention scores
  Matrix *Q = matrix_multiply(token_embedding_matrix, Wq);
  Matrix *K = matrix_multiply(token_embedding_matrix, Wk);
  Matrix *V = matrix_multiply(token_embedding_matrix, Wv);

  Matrix **Q_heads = split_matrix(Q, n_heads);
  Matrix **K_heads = split_matrix(K, n_heads);
  Matrix **V_heads = split_matrix(V, n_heads);

  matrix_free(Q);
  matrix_free(K);
  matrix_free(V);

  Matrix **context_heads = malloc(n_heads * sizeof(Matrix *));

  for (size_t h = 0; h < n_heads; h++) {
    Matrix *Qh = Q_heads[h];
    Matrix *Kh = K_heads[h];
    Matrix *Vh = V_heads[h];

    Matrix *Kh_T = matrix_transpose(Kh);
    Matrix *scores = matrix_multiply(Qh, Kh_T);
    apply_causal_mask(scores);

    // normalize by dimension of key/query vectors (row-wise in K)
    // to scale gradients
    size_t d_k = Kh->cols; // d_k == d_head
    for (size_t i = 0; i < scores->rows; i++) {
      for (size_t j = 0; j < scores->cols; j++) {
        matrix_set(scores, i, j, matrix_get(scores, i, j) / sqrt((double)d_k));
      }
    }

    // normalize with softmax
    for (size_t i = 0; i < scores->rows; i++) {
      // shift the scores to (-INF, 0]
      // avoid NaNs in exp()
      double max_val = -INFINITY;
      for (size_t j = 0; j < scores->cols; j++) {
        if (matrix_get(scores, i, j) > max_val) {
          max_val = matrix_get(scores, i, j);
        }
      }

      double sigma = 0;
      for (size_t j = 0; j < scores->cols; j++) {
        sigma += exp(matrix_get(scores, i, j) - max_val);
      }

      for (size_t j = 0; j < scores->cols; j++) {
        double normalized = exp(matrix_get(scores, i, j) - max_val) / sigma;
        matrix_set(scores, i, j, normalized);
      }
    }

    apply_dropout_mask(scores, dropout_rate);
    Matrix *context = matrix_multiply(scores, Vh);

    context_heads[h] = context;

    matrix_free(scores);
    matrix_free(Kh_T);

    // free heads as they are no longer needed
    matrix_free(Qh);
    matrix_free(Kh);
    matrix_free(Vh);
  }

  Matrix *context_vec_concat = concatenate_tensor(context_heads, n_heads);

  for (size_t i = 0; i < n_heads; i++) {
    matrix_free(context_heads[i]);
  }

  free(context_heads);
  free(Q_heads);
  free(K_heads);
  free(V_heads);

  Matrix *output = matrix_multiply(context_vec_concat, Wo);
  matrix_free(context_vec_concat);
  return output;
}

void apply_causal_mask(Matrix *scores) {
  for (size_t i = 0; i < scores->rows; i++) {
    for (size_t j = 0; j < scores->cols; j++) {
      if (j > i) {
        matrix_set(scores, i, j, -INFINITY);
      }
    }
  }
}

void apply_dropout_mask(Matrix *scores, double dropout_rate) {
  for (size_t i = 0; i < scores->rows; i++) {
    for (size_t j = 0; j < scores->cols; j++) {
      if (rand() / (double)RAND_MAX < dropout_rate) {
        matrix_set(scores, i, j, 0);
      } else {
        // scale the scores by 1/(1-p)
        matrix_set(scores, i, j, matrix_get(scores, i, j) / (1 - dropout_rate));
      }
    }
  }
}
