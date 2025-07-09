#ifndef ARRAY
#define ARRAY

#include <stdlib.h>

typedef struct {
  void *data;
  size_t len, cap, size;
  void (*destructor)(void *);
} Array;

Array *arr_init(size_t elem_size);

void arr_append(Array *arr, void *elem);

void arr_free(Array *arr);

int arr_cmp(Array *a, Array *b);

// create a dynamic array version of a C array
Array *arr_dup(void *arr, size_t len, size_t size);

// create a joined array
Array *arr_join(Array *a, Array *b);

#endif
