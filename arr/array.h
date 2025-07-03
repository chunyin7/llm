#ifndef ARRAY
#define ARRAY

#include <stdlib.h>

typedef struct {
  void *data;
  size_t len, cap, size;
} Array;

Array *arr_init(size_t elem_size);

void arr_append(Array *arr, void *elem);

void arr_free(Array *arr);

#endif
