#include "token/token.h"
#include "arr/array.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* Load entire file into a NUL‐terminated buffer. */
char *read_file(const char *path) {
  FILE *f = fopen(path, "rb");
  if (!f) return NULL;
  if (fseek(f, 0, SEEK_END)) { fclose(f); return NULL; }
  long len = ftell(f);
  fseek(f, 0, SEEK_SET);

  char *buf = malloc(len + 1);
  if (!buf) { fclose(f); return NULL; }
  if (fread(buf, 1, len, f) != (size_t)len) {
    free(buf);
    fclose(f);
    return NULL;
  }
  buf[len] = '\0';
  fclose(f);
  return buf;
}

int main(void) {
  char *str = read_file("./token/theverdict.txt");
  Vocabulary *voc = bpe(512, (uint8_t *)str, strlen(str));
  Array *tl = arr_init(sizeof(Token));
  tokenize(tl, str, strlen(str), voc);

  char *str2 = "Hello, do you like tea?";
  Array *tokens = arr_init(sizeof(Token));
  tokenize(tokens, str2, strlen(str2), voc);

  for (size_t i = 0; i < tokens->len; i++) {
    Token tok = ((Token *)tokens->data)[i];
    for (size_t j = 0; j < tok.ids->len; j++) {
      printf("'");
      if (((uint16_t *)tok.ids->data)[j] < 256) {
        printf("%c", ((uint16_t *)tok.ids->data)[j]);
      }
      printf("' ");
    }
  }
  printf("\n");

  arr_free(tokens);
  free(str);
  arr_free(tl);
  voc_free(voc);
  return 0;
}
