#include <loader/data_loader.h>
#include <stdlib.h>

DataLoader *dl_init(Array *ids, size_t seq_len, size_t batch_size) {
  DataLoader *dl = malloc(sizeof(DataLoader));
  dl->ids = ids;
  dl->seq_len = seq_len;
  dl->batch_size = batch_size;
  dl->cursor = 0;

  return dl;
}

void dl_free(DataLoader *dl) {
  // dataloader owns ids
  arr_free(dl->ids);
  free(dl);
}

Array *emit_batch(DataLoader *dl) {
  Array *batch = arr_init(sizeof(Track));
  size_t seg_len = dl->ids->len / dl->batch_size;

  if (dl->cursor >= seg_len - dl->seq_len) {
    return NULL;
  }

  for (size_t i = 0; i < dl->batch_size; i++) {
    Track t = {
      .ids = dl->ids,
      .start = dl->cursor + i * seg_len,
      .end = dl->cursor + dl->seq_len + i * seg_len
    };

    arr_append(batch, &t);
  }

  dl->cursor++;

  return batch;
}
