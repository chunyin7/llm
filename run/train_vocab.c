#include <token/tokenizer.h>
#include <token/vocabulary.h>
#include <util/array.h>
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

int main(int argc, char **argv) {
  if (argc != 3) {
    printf("Usage: %s <text_file> <vocab_size>\n", argv[0]);
  }

  char *path = argv[1];
  size_t voc_size = atoi(argv[2]);

  char *str = read_file(path);
  Vocabulary *voc = bpe(voc_size, (uint8_t *)str, strlen(str));
  voc_save(voc);
  free(str);
  voc_free(voc);
  return 0;
}
