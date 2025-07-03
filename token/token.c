// word tokenizer
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "token.h"
#include "../hashmap/hashmap.h"
#include "../arr/array.h"

Vocabulary *voc_init() {
  Vocabulary *voc = malloc(sizeof(Vocabulary));
  voc->t2i = map_init();
  voc->i2t = arr_init(sizeof(char *));

  return voc;
}

void build_voc(Array *tl, Vocabulary *voc) {
  for (size_t i = 0; i < tl->len; i++) {
    int r = map_get_or_add(voc->t2i, ((char **)tl->data)[i]);
    if (r == voc->i2t->len) {
      char *tmp = malloc(sizeof(char) * (strlen(((char **)tl->data)[i]) + 1));
      strcpy(tmp, ((char **)tl->data)[i]);
      arr_append(voc->i2t, &tmp);
    }
  }

  return;
}

void tokenize(Array *tokens, char *str, size_t len) {
  size_t start = 0;

  for (size_t end = 0; end < len; end++) {
    if (isalnum(str[end])) {
      continue;
    } else {
      // punctuation or whitespace
      // first add the entire word preceding the punctuation
      if (end - start > 0) {
        char *tmp = malloc(sizeof(char) * (end - start + 1));
        memcpy(tmp, str + start, end - start);
        tmp[end - start] = '\0';
        arr_append(tokens, &tmp);
      }

      // then add the punctuation
      char *tmp = malloc(sizeof(char) * 2);
      tmp[0] = str[end];
      tmp[1] = '\0';
      arr_append(tokens, &tmp);

      start = end + 1;
    }
  }

  if (start != len) {
    char *tmp = malloc(sizeof(char) * (len - start + 1));
    memcpy(tmp, str + start, len - start);
    tmp[len - start] = '\0';
    arr_append(tokens, &tmp);
  }

  return;
}

Array *encode(char *str, Vocabulary *voc) {
  Array *tl = arr_init(sizeof(char *));
  tokenize(tl, str, strlen(str));

  // convert token list to ids
  Array *ids = arr_init(sizeof(long));
  for (size_t i = 0; i < tl->len; i++) {
    long id = map_get_or_add(voc->t2i, ((char **) tl->data)[i]);
    arr_append(ids, &id);
  }

  arr_free(tl);
  return ids;
}

char *decode(Array *ids, Vocabulary *voc) {
  size_t cap = 16;
  size_t len = 0;
  char *s = malloc(sizeof(char) * cap);

  for (size_t i = 0; i < ids->len; i++) {
    char *substr = ((char **)voc->i2t->data)[((long *)ids->data)[i]];
    size_t l = strlen(substr);
    while (len + l >= cap - 1) {
      cap *= 2;
      s = realloc(s, cap);
    }

    memcpy(s + len, substr, l);
    len += l;
  }

  s[len] = '\0';
  return s;
}
