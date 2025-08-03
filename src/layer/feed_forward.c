#include <layer/feed_forward.h>
#include <stdlib.h>
#include <math.h>

void gelu(Matrix *mat) {
  for (size_t i = 0; i < mat->rows; i++) {
    for (size_t j = 0; j < mat->cols; j++) {
      double val = matrix_get(mat, i, j);
      val = 0.5 * val * (1 + tanh(
        sqrt(2 / M_PI) * (val + 0.044715 * pow(val, 3))
      ));
      matrix_set(mat, i, j, val);
    }
  }
}

FeedForward *feed_forward_init(size_t emb_dim) {
  FeedForward *ff = malloc(sizeof(FeedForward));
  ff->W1 = matrix_init(emb_dim, 4 * emb_dim);
  ff->W2 = matrix_init(4 * emb_dim, emb_dim);
  ff->b1 = matrix_init(4 * emb_dim, 1);
  ff->b2 = matrix_init(emb_dim, 1);

  matrix_randomize(ff->W1);
  matrix_randomize(ff->W2);
  matrix_randomize(ff->b1);
  matrix_randomize(ff->b2);

  return ff;
}

void feed_forward_free(FeedForward *ff) {
  matrix_free(ff->W1);
  matrix_free(ff->W2);
  matrix_free(ff->b1);
  matrix_free(ff->b2);
  free(ff);
}

Matrix *feed_forward_forward(FeedForward *ff, Matrix *in) {
  Matrix *hidden = matrix_multiply(in, ff->W1);

  for (size_t i = 0; i < hidden->rows; i++) {
    for (size_t j = 0; j < hidden->cols; j++) {
      double val = matrix_get(hidden, i, j) + matrix_get(ff->b1, j, 0);
      matrix_set(hidden, i, j, val);
    }
  }

  gelu(hidden);

  Matrix *out = matrix_multiply(hidden, ff->W2);

  for (size_t i = 0; i < out->rows; i++) {
    for (size_t j = 0; j < out->cols; j++) {
      double val = matrix_get(out, i, j) + matrix_get(ff->b2, j, 0);
      matrix_set(out, i, j, val);
    }
  }

  matrix_free(hidden);

  return out;
}
