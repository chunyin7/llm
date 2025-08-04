#include <transformer/layer_norm.h>
#include <util/array.h>
#include <util/matrix.h>
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

Matrix *layer_norm_forward(LayerNorm *ln, Matrix *in) {
  Matrix *out = matrix_init(in->rows, in->cols);
  
  // layer norm each row
  for (size_t i = 0; i < in->rows; i++) {
    // calculate mean
    double mean = 0;

    for (size_t j = 0; j < in->cols; j++) {
      mean += matrix_get(in, i, j);
    }

    mean /= in->cols;

    // compute variance
    double var = 0;

    for (size_t j = 0; j < in->cols; j++) {
      var += pow(matrix_get(in, i, j) - mean, 2);
    }

    var /= in->cols; // biased estimate of variance

    // normalize
    for (size_t j = 0; j < in->cols; j++) {
      double val = matrix_get(in, i, j);
      val -= mean;
      val /= sqrt(var + ln->eps);
      val *= ((double *)ln->scale->data)[j];
      val += ((double *)ln->shift->data)[j];
      matrix_set(out, i, j, val);
    }
  }
  
  return out;
}
