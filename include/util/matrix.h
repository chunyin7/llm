#ifndef MATRIX
#define MATRIX

#include <stdlib.h>

typedef struct {
  size_t rows;
  size_t cols;
  double *data; // flat array for faster indexing
} Matrix;

Matrix *matrix_init(size_t rows, size_t cols);

void matrix_free(Matrix *mat);

void matrix_set(Matrix *mat, size_t row, size_t col, double val);

double matrix_get(Matrix *mat, size_t row, size_t col);

void matrix_randomize(Matrix *mat);

Matrix *matrix_transpose(Matrix *mat);

Matrix *matrix_multiply(Matrix *a, Matrix *b);

Matrix *matrix_add(Matrix *a, Matrix *b);

#endif
