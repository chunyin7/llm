#include <token/tokenizer.h>
#include <stdio.h>
#include <string.h>

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
  if (argc != 2) {
    printf("Usage: %s <text_file>\n", argv[0]);
  }
  
  // load vocabulary
  Vocabulary *voc = voc_load("bin/vocabulary");

  if (!voc) {
    printf("Error: could not load vocabulary\n");
    return 1;
  }

  char *path = argv[1];
  char *str = read_file(path);
  Array *ids = encode((uint8_t *)str, strlen(str), voc);

  if (!ids) {
    printf("Error: could not encode string\n");
    return 1;
  }

  save_token_sequence(ids);

  free(str);
  arr_free(ids);
  voc_free(voc);
  return 0;
}
