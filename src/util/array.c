#include <util/array.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

Array *arr_init(size_t elem_size) {
  Array *arr = malloc(sizeof(Array));
  arr->len = 0;
  arr->cap = 16;
  arr->size = elem_size;
  arr->data = malloc(arr->cap * elem_size);
  arr->destructor = NULL;

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
  if (arr->destructor) {
    for (size_t i = 0; i < arr->len; i++) {
      arr->destructor((char *)arr->data + i * arr->size);
    }
  }

  free(arr->data);
  free(arr);
}

int arr_cmp(Array *a, Array *b) {
  if (a == NULL || b == NULL) {
    if (a == b) {
      return 1;
    }

    return 0;
  }

  if (a->size != b->size || a->len != b->len) {
    return 0;
  }

  for (size_t i = 0; i < a->len; i++) {
    if (memcmp((char *)a->data + i * a->size, (char *)b->data + i * b->size, a->size) != 0) {
      return 0;
    }
  }

  return 1;
}

Array *arr_dup(void *arr, size_t len, size_t size) {
  Array *cpy = arr_init(size);
  for (size_t i = 0; i < len; i++) {
    arr_append(cpy, (char *)arr + size * i);
  }

  return cpy;
}

Array *arr_join(Array *a, Array *b) {
  if (a->size != b->size) {
    return NULL;
  }

  Array *arr = arr_init(a->size);

  for (size_t i = 0; i < a->len; i++) {
    arr_append(arr, (char *)a->data + a->size * i);
  }

  for (size_t i = 0; i < b->len; i++) {
    arr_append(arr, (char *)b->data + b->size * i);
  }

  return arr;
}

float dot_float(Array *a, Array *b) {
  // assume float
  if (a->size != b->size || a->len != b->len || a->size != sizeof(float)) {
    printf("dot_float: invalid array sizes\n");
    exit(1);
  }

  float sum = 0;

  for (size_t i = 0; i < a->len; i++) {
    sum += ((float *)a->data)[i] * ((float *)b->data)[i];
  }

  return sum;
}

double dot_double(Array *a, Array *b) {
  // assume double
  if (a->size != b->size || a->len != b->len || a->size != sizeof(double)) {
    printf("dot_double: invalid array sizes\n");
    exit(1);
  }

  double sum = 0;

  for (size_t i = 0; i < a->len; i++) {
    sum += ((double *)a->data)[i] * ((double *)b->data)[i];
  }

  return sum;
}

void arr_apply(Array *a, void (*f)(void *)) {
  for (size_t i = 0; i < a->len; i++) {
    f((char *)a->data + i * a->size);
  }
}
