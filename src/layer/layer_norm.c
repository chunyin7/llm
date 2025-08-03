#include <layer/layer_norm.h>
#include <util/array.h>
#include <math.h>

LayerNorm *layer_norm_init(double eps, size_t emb_dim) {
  LayerNorm *ln = malloc(sizeof(LayerNorm));
  ln->eps = eps;
  ln->scale = arr_init(sizeof(double));
  ln->shift = arr_init(sizeof(double));

  double one = 1.0;
  double zero = 0.0;

  for (size_t i = 0; i < emb_dim; i++) {
    arr_append(ln->scale, &one);
    arr_append(ln->shift, &zero);
  }

  return ln;
}

void layer_norm_free(LayerNorm *ln) {
  arr_free(ln->scale);
  arr_free(ln->shift);
  free(ln);
}

void layer_norm_forward(LayerNorm *ln, Array *x) {
  // calculate mean
  double mean = 0;

  for (size_t i = 0; i < x->len; i++) {
    mean += ((double *)x->data)[i];
  }

  mean /= x->len;

  // compute variance
  double var = 0;

  for (size_t i = 0; i < x->len; i++) {
    var += pow(((double *)x->data)[i] - mean, 2);
  }

  var /= x->len; // biased estimate of variance

  // normalize
  for (size_t i = 0; i < x->len; i++) {
    ((double *)x->data)[i] -= mean;
    ((double *)x->data)[i] /= sqrt(var + ln->eps);
    ((double *)x->data)[i] *= ((double *)ln->scale->data)[i];
    ((double *)x->data)[i] += ((double *)ln->shift->data)[i];
  }
}
