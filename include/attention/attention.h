#ifndef ATTENTION
#define ATTENTION

#include <util/array.h>
#include <util/matrix.h>

// compute self-attention
Matrix *forward_pass(Matrix *token_embedding_matrix, Matrix *Wq, Matrix *Wk, Matrix *Wv, Matrix *causal_mask);

Matrix *init_causal_mask(size_t seq_len);

#endif
