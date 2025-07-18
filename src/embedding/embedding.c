#include <embedding/embedding.h>
#include <arr/array.h>
#include <stdlib.h>
#include <time.h>

Array *init_embedding(size_t dim, size_t voc_size) {
  Array *emb = arr_init(sizeof(float));
  srand(time(NULL));

  for (size_t i = 0; i < voc_size; i++) {
    float val = (float)rand() / (float)RAND_MAX;
    arr_append(emb, &val);
  }

  return emb;
}
