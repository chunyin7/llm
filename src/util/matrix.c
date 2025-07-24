#include <stdlib.h>
#include <util/matrix.h>

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
