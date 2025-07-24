#include <attention/attention.h>
#include <util/array.h>
#include <util/matrix.h>
#include <math.h>

Matrix *self_attention(Matrix *token_embedding_matrix, Matrix *Wq, Matrix *Wk, Matrix *Wv) {
  // compute attention scores
  Matrix *Q = matrix_multiply(token_embedding_matrix, Wq);
  Matrix *K = matrix_multiply(token_embedding_matrix, Wk);
  Matrix *V = matrix_multiply(token_embedding_matrix, Wv);

  Matrix *K_T = matrix_transpose(K);

  // scores = Q * K^T
  Matrix *scores = matrix_multiply(Q, K_T);

  // normalize with softmax
  double sigma = 0;
  for (size_t i = 0; i < scores->rows; i++) {
    for (size_t j = 0; j < scores->cols; j++) {
      sigma += exp(((double *)scores->data)[i * scores->cols + j]);
    }
  }

  for (size_t i = 0; i < scores->rows; i++) {
    for (size_t j = 0; j < scores->cols; j++) {
      double normalized = matrix_get(scores, i, j) / sigma;
      matrix_set(scores, i, j, normalized);
    }
  }

  // weight with V for context matrix
  Matrix *context = matrix_multiply(scores, V);

  return context;
}
