#include <stdlib.h>
#include <util/matrix.h>
#include <stdio.h>

Matrix *matrix_init(size_t rows, size_t cols) {
  Matrix *mat = malloc(sizeof(Matrix));
  mat->rows = rows;
  mat->cols = cols;
  mat->data = calloc(rows * cols, sizeof(double)); // calloc to zero

  return mat;
}

void matrix_free(Matrix *mat) {
  free(mat->data);
  free(mat);
}

void matrix_set(Matrix *mat, size_t row, size_t col, double val) {
  mat->data[row * mat->cols + col] = val;
}

double matrix_get(Matrix *mat, size_t row, size_t col) {
  return mat->data[row * mat->cols + col];
}

void matrix_randomize(Matrix *mat) {
  for (size_t i = 0; i < mat->rows * mat->cols; i++) {
    mat->data[i] = (double)rand() / (double)RAND_MAX;
  }
}

Matrix *matrix_transpose(Matrix *mat) {
  Matrix *new = matrix_init(mat->cols, mat->rows);

  for (size_t i = 0; i < mat->rows; i++) {
    for (size_t j = 0; j < mat->cols; j++) {
      matrix_set(new, j, i, matrix_get(mat, i, j));
    }
  }

  return new;
}

Matrix *matrix_multiply(Matrix *a, Matrix *b) {
  if (a->cols != b->rows) {
    printf("matrix_multiply: invalid dimensions\n");
    return NULL;
  }

  Matrix *new = matrix_init(a->rows, b->cols);

  for (size_t i = 0; i < a->rows; i++) {
    for (size_t j = 0; j < b->cols; j++) {
      double sum = 0;
      for (size_t k = 0; k < a->cols; k++) {
        sum += matrix_get(a, i, k) * matrix_get(b, k, j);
      }

      matrix_set(new, i, j, sum);
    }
  }

  return new;
}

Matrix *matrix_add(Matrix *a, Matrix *b) {
  if (a->rows != b->rows || a->cols != b->cols) {
    printf("matrix_add: invalid dimensions\n");
    return NULL;
  }

  Matrix *new = matrix_init(a->rows, a->cols);
  for (size_t i = 0; i < a->rows; i++) {
    for (size_t j = 0; j < a->cols; j++) {
      matrix_set(new, i, j, matrix_get(a, i, j) + matrix_get(b, i, j));
    }
  }

  return new;
}

Matrix *arr_to_matrix(Array *arr) {
  if (arr->size != sizeof(double)) {
    printf("arr_to_matrix: invalid array type\n");
    return NULL;
  }

  // convert to a row vector
  Matrix *mat = matrix_init(arr->len, 1);
  for (size_t i = 0; i < arr->len; i++) {
    matrix_set(mat, i, 0, ((double *)arr->data)[i]);
  }

  return mat;
}

Array *matrix_to_arr(Matrix *mat) {
  if (mat->cols != 1) {
    printf("matrix_to_arr: invalid dimensions\n");
    return NULL;
  }

  Array *arr = arr_init(sizeof(double));
  for (size_t i = 0; i < mat->rows; i++) {
    double val = matrix_get(mat, i, 0);
    arr_append(arr, &val);
  }

  return arr;
}

Matrix *matrix_dup(Matrix *mat) {
  Matrix *new = matrix_init(mat->rows, mat->cols);
  for (size_t i = 0; i < mat->rows; i++) {
    for (size_t j = 0; j < mat->cols; j++) {
        matrix_set(new, i, j, matrix_get(mat, i, j));
    }
  }
  return new;
}
