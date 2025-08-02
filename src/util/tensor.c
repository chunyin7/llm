#include <util/tensor.h>
#include <util/matrix.h>
#include <stdlib.h>
#include <stdio.h>

Matrix **split_matrix(Matrix *mat, size_t n_heads) {
  if (mat->cols % n_heads != 0) {
    printf("Matrix columns must be divisible by n_heads\n");
    exit(1);
  }

  size_t d_head = mat->cols / n_heads;
  size_t seq_len = mat->rows;
  Matrix **heads = malloc(n_heads * sizeof(Matrix *));

  for (size_t i = 0; i < n_heads; i++) {
    heads[i] = matrix_init(seq_len, d_head);
    for (size_t j = 0; j < seq_len; j++) {
      for (size_t k = 0; k < d_head; k++) {
        matrix_set(heads[i], j, k, matrix_get(mat, j, i * d_head + k));
      }
    }
  }

  return heads;
}

Matrix *concatenate_tensor(Matrix **tensor, size_t n_heads) {
  size_t seq_len = tensor[0]->rows;
  size_t d_head = tensor[0]->cols;

  Matrix *mat = matrix_init(seq_len, n_heads * d_head);

  for (size_t i = 0; i < n_heads; i++) {
    for (size_t j = 0; j < seq_len; j++) {
      for (size_t k = 0; k < d_head; k++) {
        matrix_set(mat, j, i * d_head + k, matrix_get(tensor[i], j, k));
      }
    }
  }

  return mat;
}
