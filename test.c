#include "token/token.h"
#include "hashmap/hashmap.h"
#include "arr/array.h"
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

int main(void) {
  char *str = read_file("./token/theverdict.txt");
  Array *tl = arr_init(sizeof(char *));
  tokenize(tl, str, strlen(str));

  Vocabulary *voc = voc_init();
  build_voc(tl, voc);

  Array *ids = encode(str, voc);
  char *s = decode(ids, voc);
  printf("%s\n", s);

  free(str);
  free(s);
  free(tl);
  free(ids);
  free(voc);
  return 0;
}
