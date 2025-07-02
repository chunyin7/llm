// word tokenizer
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "token.h"
#include "../hashmap/hashmap.h"

TokenList *tl_init() {
  TokenList *tl = malloc(sizeof(TokenList));
  tl->len = 0;
  tl->cap = 16;
  tl->data = malloc(tl->cap * sizeof(char *));

  return tl;
}

IDList *id_init() {
  IDList *ids = malloc(sizeof(TokenList));
  ids->len = 0;
  ids->cap = 16;
  ids->data = malloc(ids->cap * sizeof(long));

  return ids;
}

Vocabulary *voc_init() {
  Vocabulary *voc = malloc(sizeof(Vocabulary));
  voc->t2i = map_init();
  voc->i2t = tl_init();

  return voc;
}

void build_voc(TokenList *tl, Vocabulary *voc) {
  for (size_t i = 0; i < tl->len; i++) {
    int r = map_get_or_add(voc->t2i, tl->data[i]);
    if (r == voc->i2t->len) {
      tl_add(voc->i2t, tl->data[i], strlen(tl->data[i]));
    }
  }

  return;
}

void tl_add(TokenList *tl, char *tok, size_t tok_len) {
  if (tl->len >= tl->cap - 1) {
    tl->cap *= 2;
    tl->data = realloc(tl->data, tl->cap * sizeof(char *));
  }

  tl->data[tl->len] = malloc(sizeof(char) * (tok_len + 1));
  memcpy(tl->data[tl->len], tok, tok_len);
  tl->data[tl->len][tok_len] = '\0';
  tl->len++;

  return;
}

void id_add(IDList *ids, long id) {
  if (ids->len >= ids->cap - 1) {
    ids->cap *= 2;
    ids->data = realloc(ids->data, ids->cap * sizeof(long));
  }

  ids->data[ids->len] = id;
  ids->len++;

  return;
}

void tl_free(TokenList *tl) {
  for (size_t i = 0; i < tl->len; i++) {
    free(tl->data[i]);
  }

  free(tl->data);
  free(tl);

  return;
}

void id_free(IDList *ids) {
  free(ids->data);
  free(ids);

  return;
}

void tokenize(TokenList *tl, char *str, size_t len) {
  size_t start = 0;

  for (size_t end = 0; end < len; end++) {
    if (isalnum(str[end])) {
      continue;
    } else if (isspace(str[end])) {
      if (start != end) {
        char *tmp = malloc(sizeof(char) * (end - start + 1));
        memcpy(tmp, str + start, end - start);
        tmp[end - start] = '\0';
        tl_add(tl, tmp, end - start);
        free(tmp);
      }

      start = end + 1;
    } else {
      // punctuation
      // first add the entire word preceding the punctuation
      if (end - start > 1) {
        char *tmp = malloc(sizeof(char) * (end - start + 1));
        memcpy(tmp, str + start, end - start);
        tmp[end - start] = '\0';
        tl_add(tl, tmp, end - start);
        free(tmp);
      }

      // then add the punctuation
      char tmp[2];
      tmp[0] = str[end];
      tmp[1] = '\0';
      tl_add(tl, tmp, 1);

      start = end + 1;
    }
  }

  if (start != len) {
    char *tmp = malloc(sizeof(char) * (len - start + 1));
    memcpy(tmp, str + start, len - start);
    tmp[len - start] = '\0';
    tl_add(tl, tmp, len - start);
    free(tmp);
  }

  return;
}

// util / debug
void tl_print(TokenList *tl) {
  printf("TOKEN LIST, length: %zu, capacity: %zu:\n", tl->len, tl->cap);
  printf("[ ");
  for (size_t i = 0; i < tl->len; i++) {
    printf("'%s' ", tl->data[i]);
  }
  printf(" ]\n");

  return;
}

void id_print(IDList *ids) {
  printf("TOKEN LIST, length: %zu, capacity: %zu:\n", ids->len, ids->cap);
  printf("[ ");
  for (size_t i = 0; i < ids->len; i++) {
    printf("'%ld' ", ids->data[i]);
  }
  printf(" ]\n");

  return;
}

IDList *encode(char *str, Vocabulary *voc) {
  TokenList *tl = tl_init();
  tokenize(tl, str, strlen(str));

  // convert token list to ids
  IDList *ids = id_init();
  for (size_t i = 0; i < tl->len; i++) {
    id_add(ids, map_get_or_add(voc->t2i, tl->data[i]));
  }

  tl_free(tl);
  return ids;
}

