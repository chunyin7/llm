#include <attention/attention.h>
#include <util/array.h>
#include <util/matrix.h>
#include <math.h>

Matrix *forward_pass(Matrix *token_embedding_matrix, Matrix *Wq, Matrix *Wk, Matrix *Wv) {
  // compute attention scores
  Matrix *Q = matrix_multiply(token_embedding_matrix, Wq);
  Matrix *K = matrix_multiply(token_embedding_matrix, Wk);
  Matrix *V = matrix_multiply(token_embedding_matrix, Wv);

  Matrix *K_T = matrix_transpose(K);

  // scores = Q * K^T
  Matrix *scores = matrix_multiply(Q, K_T);
  apply_causal_mask(scores);

  // normalize by dimension of key/query vectors (row-wise in K)
  // to scale gradients
  size_t d_k = K->cols;

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

  // weight with V for context matrix
  Matrix *context = matrix_multiply(scores, V);

  matrix_free(scores);
  matrix_free(K_T);
  matrix_free(Q);
  matrix_free(K);
  matrix_free(V);

  return context;
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
