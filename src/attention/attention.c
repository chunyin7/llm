#include <attention/attention.h>
#include <util/array.h>
#include <util/matrix.h>
#include <math.h>

Matrix *forward_pass(Matrix *token_embedding_matrix, Matrix *Wq, Matrix *Wk, Matrix *Wv, Matrix *causal_mask) {
  // compute attention scores
  Matrix *Q = matrix_multiply(token_embedding_matrix, Wq);
  Matrix *K = matrix_multiply(token_embedding_matrix, Wk);
  Matrix *V = matrix_multiply(token_embedding_matrix, Wv);

  Matrix *K_T = matrix_transpose(K);

  // scores = Q * K^T
  Matrix *scores = matrix_multiply(Q, K_T);
  Matrix *masked = matrix_add(scores, causal_mask);

  // normalize by dimension of key/query vectors (row-wise in K)
  // to scale gradients
  size_t d_k = K->cols;

  for (size_t i = 0; i < masked->rows; i++) {
    for (size_t j = 0; j < masked->cols; j++) {
      matrix_set(masked, i, j, matrix_get(masked, i, j) / sqrt((double)d_k));
    }
  }

  // normalize with softmax
  double sigma = 0;
  for (size_t i = 0; i < masked->rows; i++) {
    for (size_t j = 0; j < masked->cols; j++) {
      sigma += exp(matrix_get(masked, i, j));
    }
  }

  for (size_t i = 0; i < masked->rows; i++) {
    for (size_t j = 0; j < masked->cols; j++) {
      double normalized = exp(matrix_get(masked, i, j)) / sigma;
      matrix_set(masked, i, j, normalized);
    }
  }

  // weight with V for context matrix
  Matrix *context = matrix_multiply(masked, V);

  matrix_free(scores);
  matrix_free(K_T);
  matrix_free(Q);
  matrix_free(K);
  matrix_free(V);

  return context;
}

Matrix *init_causal_mask(size_t seq_len) {
  Matrix *mask = matrix_init(seq_len, seq_len);

  for (size_t i = 0; i < seq_len; i++) {
    for (size_t j = 0; j < seq_len; j++) {
      if (i < j) {
        matrix_set(mask, i, j, -INFINITY);
      } else {
        matrix_set(mask, i, j, 0);
      }
    }
  }

  return mask;
}
