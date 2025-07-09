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
  Vocabulary *voc = bpe(1024, (uint8_t *)str, strlen(str));
  voc_save(voc);
  free(str);
  voc_free(voc);
  return 0;
}
