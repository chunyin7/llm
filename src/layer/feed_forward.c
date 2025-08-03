#include <layer/feed_forward.h>
#include <math.h>

void gelu(Array *x) {
  for (size_t i = 0; i < x->len; i++) {
    double val = ((double *)x->data)[i];
    val = 0.5 * val * (1 + tanh(
      sqrt(2 / M_PI) * (val + 0.044715 * pow(val, 3))
    ));
    ((double *)x->data)[i] = val;
  }
}
