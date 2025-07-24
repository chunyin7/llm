#include <attention/attention.h>
#include <util/array.h>
#include <util/matrix.h>
#include <math.h>

Array *compute_self_attention_scores(Array *mat) {
  Array *scores = arr_init(sizeof(double));

  for (size_t i = 0; i < mat->len; i++) {
    Array *q = ((Array **)mat->data)[i];
    for (size_t j = 0; j < mat->len; j++) {
      Array *k = ((Array **)mat->data)[j];
      double score = dot_double(q, k);
      arr_append(scores, &score);
    }
  }

  return scores;
}

void softmax(Array *scores) {
  double sigma = 0;

  for (size_t i = 0; i < scores->len; i++) {
    sigma += exp(((double *)scores->data)[i]);
  }

  for (size_t i = 0; i < scores->len; i++) {
    double val = exp(((double *)scores->data)[i]) / sigma;
    ((double *)scores->data)[i] = val;
  }
}

Array *compute_context_mat(Array *weights, Array *embeddings, size_t dim) {
  Array *mat = arr_init(sizeof(Array *));

  for (size_t i = 0; i < weights->len; i++) {
    Array *context_vec = arr_init(sizeof(double));

    for (size_t j = 0; j < dim; j++) {
      double val = 0;
      arr_append(context_vec, &val);
    }

    for (size_t j = 0; j < weights->len; j++) {
      double weight = ((double *)weights->data)[j];
      Array *emb = ((Array **)embeddings->data)[j];

      for (size_t k = 0; k < dim; k++) {
        double val = ((double *)emb->data)[k] * weight;
        ((double *)context_vec->data)[k] += val;
      }
    }

    arr_append(mat, &context_vec);
  }

  return mat;
}
