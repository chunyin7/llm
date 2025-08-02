#include <util/matrix.h>

// split a matrix into n_heads matrices
// split by column
Matrix **split_matrix(Matrix *mat, size_t n_heads);

Matrix *concatenate_tensor(Matrix **tensor, size_t n_heads);
