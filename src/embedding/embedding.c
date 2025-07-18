#include <embedding/embedding.h>
#include <arr/array.h>
#include <stdlib.h>
#include <time.h>

Array *init_embedding(size_t dim, size_t voc_size) {
  Array *emb = arr_init(sizeof(Array *));
  srand(time(NULL));

  for (size_t i = 0; i < voc_size; i++) {
    Array *tok = arr_init(sizeof(float));
    for (size_t j = 0; j < dim; j++) {
      float val = (float)rand() / (float)RAND_MAX;
      arr_append(tok, &val);
    }

    arr_append(emb, &tok);
  }

  return emb;
}

Array *embed(Array *ids, Array *emb_mat, size_t dim) {
  Array *emb = arr_init(sizeof(Array *));

  for (size_t i = 0; i < ids->len; i++) {
    Array *tok_emb = arr_init(sizeof(float));
    long id = ((long *)ids->data)[i];
    Array *emb_mat_row = &((Array *)emb_mat->data)[id];

    for (size_t j = 0; j < dim; j++) {
      float val = ((float *)emb_mat_row->data)[j];
      arr_append(tok_emb, &val);
    }

    arr_append(emb, &tok_emb);
  }

  return emb;
}
