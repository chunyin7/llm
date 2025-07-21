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

// calculate dot product of two float arrays
Array *dot_float(Array *a, Array *b);

// calculate dot product of two double arrays
Array *dot_double(Array *a, Array *b);

// apply a function to each element of an array
void arr_apply(Array *a, void (*f)(void *));

#endif
