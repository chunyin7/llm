#ifndef ATTENTION
#define ATTENTION

#include <util/array.h>
#include <util/matrix.h>

// compute self-attention
Matrix *self_attention(Matrix *token_embedding_matrix, Matrix *Wq, Matrix *Wk, Matrix *Wv);

#endif
