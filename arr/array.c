#include "array.h"
#include <string.h>

Array *arr_init(size_t elem_size) {
  Array *arr = malloc(sizeof(Array));
  arr->len = 0;
  arr->cap = 16;
  arr->size = elem_size;
  arr->data = malloc(arr->cap * elem_size);

  return arr;
}

void arr_append(Array *arr, void *elem) {
  if (arr->len >= arr->cap) {
    arr->cap *= 2;
    arr->data = realloc(arr->data, arr->cap * arr->size);
  }

  memcpy((char *)arr->data + arr->len * arr->size, elem, arr->size);
  arr->len++;
}

void arr_free(Array *arr) {
  free(arr->data);
  free(arr);
}
