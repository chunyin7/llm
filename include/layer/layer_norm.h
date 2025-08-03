#ifndef LAYER_NORM
#define LAYER_NORM

#include <stdlib.h>
#include <util/array.h>

typedef struct {
  double eps;
  Array *scale;  // length = feature_dim, init to 1.0
  Array *shift;  // length = feature_dim, init to 0.0
} LayerNorm;

LayerNorm *layer_norm_init(double eps, size_t emb_dim);

void layer_norm_free(LayerNorm *ln);

void layer_norm_forward(LayerNorm *ln, Array *x);

#endif
