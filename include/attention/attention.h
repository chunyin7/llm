#ifndef ATTENTION
#define ATTENTION

#include <util/array.h>
#include <util/matrix.h>

// compute self-attention
Matrix *forward_pass(Matrix *token_embedding_matrix, Matrix *Wq, Matrix *Wk, Matrix *Wv);

void apply_causal_mask(Matrix *scores);

#endif
