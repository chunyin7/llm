#include <attention/attention.h>
#include <arr/array.h>

Array *compute_self_attention_scores(Array *vec) {
  Array *scores = arr_init(sizeof(double));

  for (size_t i = 0; i < vec->len; i++) {
    double q = ((double *)vec->data)[i];
    for (size_t j = 0; j < vec->len; j++) {
      double k = ((double *)vec->data)[j];
      double score = q * k;
      arr_append(scores, &score);
    }
  }

  return scores;
}

void normalize_scores(Array *scores) {
  // TODO: softmax?
}
