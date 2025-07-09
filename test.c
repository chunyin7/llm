#include <token/tokenizer.h>
#include <token/vocabulary.h>
#include <arr/array.h>
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
  char *str = read_file("theverdict.txt");
  Vocabulary *voc = bpe(512, (uint8_t *)str, strlen(str));

  char *str2 = "Hello, do you like tea?";
  Array *ids = encode((uint8_t *)str2, strlen(str2), voc);
  Array *bytes = decode(ids, voc);

  for (size_t i = 0; i < bytes->len; i++) {
    printf("%c", ((uint8_t *)bytes->data)[i]);
  }
  printf("\n");

  arr_free(ids);
  arr_free(bytes);
  free(str);
  voc_free(voc);
  return 0;
}
