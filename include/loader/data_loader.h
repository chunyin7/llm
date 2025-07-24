#ifndef DATA_LOADER
#define DATA_LOADER

#include <util/array.h>

typedef struct {
  Array *ids;
  size_t seq_len; // sequence length of sliding windows
  size_t batch_size;
  size_t cursor; // contiguous?
} DataLoader;

typedef struct {
  Array *ids;
  size_t start;
  size_t end;
} Track; // sliding window

DataLoader *dl_init(Array *ids, size_t seq_len, size_t batch_size);

void dl_free(DataLoader *dl);

#endif
