#include "token/token.h"
#include "hashmap/hashmap.h"
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
    TokenList *tl = tl_init();
    char *str = read_file("./token/theverdict.txt");
    tokenize(tl, str, strlen(str));
    printf("tokenized\n");

    Map *voc = map_init();
    build_voc(tl, voc);
    printf("vocab map built\n");
    voc_print(voc);

    map_free(voc);
    tl_free(tl);
    free(str);

    return 0;
}
