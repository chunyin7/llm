#ifndef FEED_FORWARD
#define FEED_FORWARD

#include <util/array.h>
#include <util/matrix.h>

typedef struct {
  Matrix *W1;
  Matrix *W2;
  Matrix *b1;
  Matrix *b2;
} FeedForward;

FeedForward *feed_forward_init(size_t emb_dim);

void feed_forward_free(FeedForward *ff);

Matrix *feed_forward_forward(FeedForward *ff, Matrix *in);

void gelu(Matrix *mat);

#endif
