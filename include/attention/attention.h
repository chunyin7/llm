#ifndef ATTENTION
#define ATTENTION

#include <arr/array.h>

// naive self-attention scores
Array *compute_self_attention_scores(Array *vec);

// normalize to produce weights
void normalize_scores(Array *scores);

#endif
